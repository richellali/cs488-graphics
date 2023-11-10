// Termm-Fall 2023

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <algorithm>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string &luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),

	  // -- GUI
	  current_widget(MODE::POSITION_ORIEN),
	  circle_box(0),
	  zBuffer_box(1),
	  backfaceCulling_box(0),
	  frontfaceCulling_box(0),

	  // -- Mouse Input
	  m_mouseButtonActive(0),
	  prev_mouse_x(0.0f),
	  prev_mouse_y(0.0f),
	  m_button{false, false, false},

	  // -- Transformation
	  areJointsRotated(0),
	  model_translate(mat4(1.0f)),
	  model_rotate(mat4(1.0f)),

	  // -- Picking
	  do_picking(false),

	  // -- undo/redo
	  curStackIdx(0),
	  curStackSize(1)

{
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);
	// glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator(new MeshConsolidator{
		getAssetFilePath("cube.obj"),
		getAssetFilePath("sphere.obj"),
		getAssetFilePath("suzanne.obj")});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	initPickingMap(*m_rootNode);

	undoRedoStack.push_back(std::vector<JointStat>());
	createUndoRedoStack(&(*m_rootNode), curStackIdx);

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string &filename)
{
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode)
	{
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader(getAssetFilePath("arc_VertexShader.vs").c_str());
	m_shader_arcCircle.attachFragmentShader(getAssetFilePath("arc_FragmentShader.fs").c_str());
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}

	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos(
	const MeshConsolidator &meshConsolidator)
{
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
					 meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
					 meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers(1, &m_vbo_arcCircle);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);

		float *pts = new float[2 * CIRCLE_PTS];
		for (size_t idx = 0; idx < CIRCLE_PTS; ++idx)
		{
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2 * idx] = cos(ang);
			pts[2 * idx + 1] = sin(ang);
		}

		glBufferData(GL_ARRAY_BUFFER, 2 * CIRCLE_PTS * sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}

//----------------------------------------------------------------------------------------
void A3::initViewMatrix()
{
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
						 vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources()
{
	// World-space position
	m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.rgbIntensity = vec3(0.7f); // light
}

//----------------------------------------------------------------------------------------
void A3::initPickingMap(SceneNode &node)
{
	// std::cout << node.m_nodeId << " " << node.m_name << '\n';
	pickingMap[node.m_nodeId] = &node;

	for (SceneNode *child : node.children)
	{
		initPickingMap(*child);
	}
}

void A3::createUndoRedoStack(SceneNode *node, int idx)
{
	if (node->m_nodeType == NodeType::JointNode)
	{
		JointNode *jointNode = static_cast<JointNode *>(node);
		undoRedoStack[idx].push_back(JointStat{jointNode, jointNode->xAngle, jointNode->yAngle, jointNode->trans, jointNode->isSelected});
		// std::cout << node->m_name << " " << jointNode->isSelected<<'\n';
	}

	for (SceneNode *child : node->children)
	{
		createUndoRedoStack(child, idx);
	}
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms()
{
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i(location, do_picking ? 1 : 0);

		if (!do_picking)
		{
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.25f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...
	if (current_widget == MODE::POSITION_ORIEN)
	{ 
		glClearColor(0.2, 0.5, 0.3, 1.0);
	} else {
		glClearColor(0.35, 0.35, 0.35, 1.0);
	}
	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if (!show_gui)
	{
		return;
	}

	static bool firstRun(true);
	if (firstRun)
	{
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100, 100), opacity,
				 windowFlags);

	// Add more gui elements here here ...
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Reset Position", "I"))
			{
				resetPos();
			}
			if (ImGui::MenuItem("Reset Orientation", "O"))
			{
				resetOrien();
			}
			if (ImGui::MenuItem("Reset Joints", "S"))
			{
				resetJoints();
			}
			if (ImGui::MenuItem("Reset All", "A"))
			{
				resetAll();
			}
			if (ImGui::MenuItem("Quit", "Q"))
			{
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "U"))
			{
				undoStack();
			}
			if (ImGui::MenuItem("Redo", "R"))
			{
				redoStack();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Checkbox("Circle (C)", &circle_box);
			ImGui::Checkbox("Z-Buffer (Z)", &zBuffer_box);
			ImGui::Checkbox("Backface Culling (B)", &backfaceCulling_box);
			ImGui::Checkbox("Frontface Culling (F)", &frontfaceCulling_box);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::PushID(1);
	ImGui::RadioButton("Position/Orientation (P)", &current_widget, MODE::POSITION_ORIEN);
	ImGui::RadioButton("Joints (J)", &current_widget, MODE::JOINTS);
	ImGui::PopID();

	// Create Button, and check if it was clicked:
	// if( ImGui::Button( "Quit Application" ) ) {
	// 	glfwSetWindowShouldClose(m_window, GL_TRUE);
	// }

	ImGui::Text("Current Stack at %d", curStackIdx);
	ImGui::Text("Stack Size %d", curStackSize);

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
	const ShaderProgram &shader,
	const GeometryNode &node,
	const glm::mat4 &viewMatrix,
	const glm::mat4 &modelMatrix,
	bool do_picking,
	bool isJointMode
	)
{

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if (do_picking)
		{
			float r = float(node.m_nodeId & 0xff) / 255.0f;
			float g = float((node.m_nodeId >> 8) & 0xff) / 255.0f;
			float b = float((node.m_nodeId >> 16) & 0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f(location, r, g, b);
			CHECK_GL_ERRORS;
		}
		else
		{
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;

			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.isSelected && isJointMode ? vec3{1.0, 1.0, 0.0} : node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
		}
	}
	shader.disable();
}


//----------------------------------------------------------------------------------------

void A3::renderGeometryNode(const SceneNode *node, glm::mat4 modelMatrix)
{

	modelMatrix = modelMatrix * node->trans;

	if (node->m_nodeType == NodeType::GeometryNode)
	{
		const GeometryNode *geometryNode = static_cast<const GeometryNode *>(node);

		updateShaderUniforms(m_shader, *geometryNode, m_view, modelMatrix, do_picking, current_widget==MODE::JOINTS);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	for (const SceneNode *child : node->children)
	{
		renderGeometryNode(child, modelMatrix);
	}
}

void A3::renderSceneGraph(const SceneNode &root)
{
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	mat4 modelMatrix = root.trans * model_translate * model_rotate;
	for (const SceneNode *node : root.children)
	{
		renderGeometryNode(node, modelMatrix);
	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw()
{

	if (zBuffer_box)
	{
		glEnable(GL_DEPTH_TEST);
	}

	if (frontfaceCulling_box && backfaceCulling_box)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else if (frontfaceCulling_box)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	else if (backfaceCulling_box)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	renderSceneGraph(*m_rootNode);

	if (frontfaceCulling_box || backfaceCulling_box)
	{
		glDisable(GL_CULL_FACE);
	}

	if (zBuffer_box)
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (circle_box) renderArcCircle();
}


//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle()
{
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
	GLint m_location = m_shader_arcCircle.getUniformLocation("M");
	float aspect = float(m_framebufferWidth) / float(m_framebufferHeight);
	glm::mat4 M;
	if (aspect > 1.0)
	{
		M = glm::scale(glm::mat4(), glm::vec3(0.5 / aspect, 0.5, 1.0));
	}
	else
	{
		M = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5 * aspect, 1.0));
	}
	glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(M));
	glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_PTS);
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{
}
//----------------------------------------------------------------------------------------
/*
 * GUI button
 */
void A3::resetPos()
{
	model_translate = mat4(1.0);
	draw();
}

void A3::resetOrien()
{
	model_rotate = mat4(1.0);
	draw();
}

// Reset all joint angles, and clear the undo/redo stack
void A3::resetJoints()
{
	undoRedoStack.resize(1);
	curStackIdx = 0;
	curStackSize = 1;
	applyCurIdxToStack();
	draw();
}

// Reset the position, orientation, and joint angles of the puppet, and clear the undo/redo stack. isSelected
void A3::resetAll()
{
	model_translate = mat4(1.0);
	model_rotate = mat4(1.0);

	undoRedoStack.resize(1);
	curStackIdx = 0;
	curStackSize = 1;
	applyCurIdxToStack();

	draw();
}

//----------------------------------------------------------------------------------------
/*
 * Undo/Redo Stack
 */
void A3::applyCurIdxToStack()
{
	for (auto &js : undoRedoStack[curStackIdx])
	{
		js.node->setXAngle(js.xAngle);
		js.node->setYAngle(js.yAngle);
		js.node->set_transform(js.trans);
		// js.node->isSelected = js.isSelected;
	}
	draw();
}

void A3::undoStack()
{
	if (curStackIdx > 0)
	{
		curStackIdx--;
		applyCurIdxToStack();
	}
}

void A3::redoStack()
{
	if (curStackIdx < curStackSize - 1)
	{
		curStackIdx++;
		applyCurIdxToStack();
	}
}

void A3::addNewStatToStack()
{
	// undoRedoStack.push_back(std::vector<JointStat>());
	// std::vector<JointStat> newJointStats;
	// for (auto node : selectedJoints)
	// {
	// 	JointNode *jointNode = static_cast<JointNode *>(node);
	// 	newJointStats.push_back(JointStat{jointNode, jointNode->xAngle, jointNode->yAngle, jointNode->trans, jointNode->isSelected});
	// }

	curStackIdx++;
	curStackSize = curStackIdx + 1;

	if (curStackSize > undoRedoStack.size())
	{
		undoRedoStack.push_back(std::vector<JointStat>());
	}
	else
	{
		undoRedoStack[curStackIdx] = std::vector<JointStat>();
	}
	createUndoRedoStack(&(*m_rootNode), curStackIdx);

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent(
	int entered)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}
//----------------------------------------------------------------------------------------
/*
 * Mouse Movement Event Handler Helper
 */

void A3::translate_puppet(double x_mov, double y_mov)
{
	double factor = 0.01f;
	mat4 translate(1.0f);

	if (m_button.left)
	{
		translate[3].x = x_mov * factor;
		translate[3].y = y_mov * factor;
	}
	else if (m_button.middle)
	{

		translate[3].z = -y_mov * factor;
	}

	model_translate = model_translate * translate;
}

// trackball tut
void trackball_vec(glm::vec3 &vec)
{
	double OP_squared = vec.x * vec.x + vec.y * vec.y;
	// std::cout << OP_squared << std::endl;
	if (OP_squared <= 1.0f)
	{
		vec.z = sqrt(1 - OP_squared);
	}
	else
	{
		vec = normalize(vec);
	}
}

void A3::rotate_puppet(double xPos, double yPos)
{
	double radius = std::min(m_windowWidth, m_windowHeight) / 4;
	double iCenterX = m_windowWidth / 2;
	double iCenterY = m_windowHeight / 2;

	vec3 S{prev_mouse_x - iCenterX, iCenterY - prev_mouse_y, 0.0f};
	vec3 T{xPos - iCenterX, iCenterY - yPos, 0.0f};
	S /= radius;
	T /= radius;

	// check if outside the circle
	trackball_vec(S);
	trackball_vec(T);
	// std::cout << "S: " << S << '\n';
	// std::cout << "T: " << T << '\n';
	vec3 rotation_vec = cross(S, T);
	float radians = sqrt(dot(rotation_vec, rotation_vec));

	if (radians > -0.000001 && radians < 0.000001)
	{
		return;
	}
	// std::cout << rotation_vec << std::endl;
	// std::cout << std::endl;
	model_rotate = model_rotate * rotate(mat4(1.0f), radians, rotation_vec);
}

// x axis -> front back
// y aixs -> left right
void A3::rotate_joints(double x_mov, double y_mov)
{
	// std::cout << "rotate joints" << std::endl;
	if (selectedJoints.size() > 0) areJointsRotated = true;
	if (m_button.right)
	{
		// std::cout << "y " << x_mov << '\n'; 
		for (auto node : selectedJoints)
		{
			node->rotate('y', x_mov);
		}
	}

	if (m_button.middle)
	{
		// std::cout << "x " << y_mov << '\n'; 
		for (auto node : selectedJoints)
		{
			node->rotate('x', y_mov);
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent(
	double xPos,
	double yPos)
{
	bool eventHandled(false);

	if (m_mouseButtonActive)
	{
		double x_mov = xPos - prev_mouse_x;
		double y_mov = prev_mouse_y - yPos;
		if (current_widget == MODE::POSITION_ORIEN)
		{

			if (m_button.left || m_button.middle)
			{
				translate_puppet(x_mov, y_mov);
			}
			else if (m_button.right)
			{
				rotate_puppet(xPos, yPos);
			}
		}
		else if (current_widget == MODE::JOINTS)
		{
			if (m_button.middle || m_button.right)
			{
				rotate_joints(x_mov, y_mov);
			}
		}
	}
	prev_mouse_x = xPos;
	prev_mouse_y = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Picking Handler. Source: picking example
 */

void A3::selectOrDisselectJoints(unsigned int nodeId, SceneNode &node)
{
	for (SceneNode *child : node.children)
	{
		if (child->m_nodeId == nodeId)
		{
			child->isSelected = !child->isSelected;

			if (node.m_nodeType == NodeType::JointNode)
			{
				JointNode *jointNode = static_cast<JointNode *>(&node);
				if (jointNode->isNotRotatable()) continue;

				if (child->isSelected)
				{
					selectedJoints.push_back(&node);
				}
				else
				{
					auto it = std::find(selectedJoints.begin(), selectedJoints.end(), &node);
					if (it != selectedJoints.end())
					{
						selectedJoints.erase(it);
					}
				}

				// std::cout << "push joint: " << node.m_name << '\n';
			}
			return;
		}

		selectOrDisselectJoints(nodeId, *child);
	}
}

void A3::mouseClickedPicking()
{
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	do_picking = true;

	uploadCommonSceneUniforms();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.35, 0.35, 0.35, 1.0);

	draw();

	// I don't know if these are really necessary anymore.
	// glFlush();
	// glFinish();

	CHECK_GL_ERRORS;

	// Ugly -- FB coordinates might be different than Window coordinates
	// (e.g., on a retina display).  Must compensate.
	xpos *= double(m_framebufferWidth) / double(m_windowWidth);
	// WTF, don't know why I have to measure y relative to the bottom of
	// the window in this case.
	ypos = m_windowHeight - ypos;
	ypos *= double(m_framebufferHeight) / double(m_windowHeight);

	GLubyte buffer[4] = {0, 0, 0, 0};
	// A bit ugly -- don't want to swap the just-drawn false colours
	// to the screen, so read from the back buffer.
	glReadBuffer(GL_BACK);
	// Actually read the pixel at the mouse location.
	glReadPixels(int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	CHECK_GL_ERRORS;

	// Reassemble the object ID.
	unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

	// if (what < pickingMap.size())
	// {
	// 	std::cout << "Picking " << what << " " << pickingMap[what]->m_name << '\n';
	// 	pickingMap[what]->isSelected = !pickingMap[what]->isSelected;
	// }
	selectOrDisselectJoints(what, *m_rootNode);

	do_picking = false;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent(
	int button,
	int actions,
	int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (actions == GLFW_PRESS)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				m_button.left = true;
				if (current_widget == MODE::JOINTS)
				{
					mouseClickedPicking();
				}
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				m_button.middle = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				m_button.right = true;
			}

			m_mouseButtonActive = true;
			eventHandled = true;
		}
	}

	if (actions == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_button.left = false;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			m_button.middle = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			m_button.right = false;
		}

		if (areJointsRotated && current_widget == MODE::JOINTS)
		{
			addNewStatToStack();
			areJointsRotated = false;
		}

		m_mouseButtonActive = false;
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent(
	double xOffSet,
	double yOffSet)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent(
	int width,
	int height)
{
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent(
	int key,
	int action,
	int mods)
{
	bool eventHandled(false);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_M)
		{
			show_gui = !show_gui;
			eventHandled = true;
		}
		// UI Application
		if (key == GLFW_KEY_I)
		{
			resetPos();
			eventHandled = true;
		}
		if (key == GLFW_KEY_O)
		{
			resetOrien();
			eventHandled = true;
		}
		if (key == GLFW_KEY_S)
		{
			resetJoints();
			eventHandled = true;
		}
		if (key == GLFW_KEY_A)
		{
			resetAll();
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}

		// GUI Edit
		if (key == GLFW_KEY_U)
		{
			undoStack();
			eventHandled = true;
		}
		if (key == GLFW_KEY_R)
		{
			redoStack();
			eventHandled = true;
		}

		// GUI Options
		{
			if (key == GLFW_KEY_C)
			{
				circle_box = (circle_box == true ? false : true);
				eventHandled = true;
			}
			if (key == GLFW_KEY_Z)
			{
				zBuffer_box = (zBuffer_box == true ? false : true);
				eventHandled = true;
			}
			if (key == GLFW_KEY_B)
			{
				backfaceCulling_box = (backfaceCulling_box == true ? false : true);
				eventHandled = true;
			}
			if (key == GLFW_KEY_F)
			{
				frontfaceCulling_box = (frontfaceCulling_box == true ? false : true);
				eventHandled = true;
			}
		}

		// GUI radio button
		{
			if (key == GLFW_KEY_P)
			{
				current_widget = MODE::POSITION_ORIEN;
				eventHandled = true;
			}
			if (key == GLFW_KEY_J)
			{
				current_widget = MODE::JOINTS;
				eventHandled = true;
			}
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
