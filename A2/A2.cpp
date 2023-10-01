// Termm--Fall 2023

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}

//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f)),
	  cube_num(24),
	  coord_num(6),

	  // UI
	  current_widget(0),

	  axis{false, false, false},

	  // mouse
	  m_mouseButtonActive(false),
	  prev_mouse_x(0.0f),

	  // matrix
	  m_translate(mat4(1.0f)),
	  m_rotate(mat4(1.0f)),
	  m_scale(mat4(1.0f)),
	  v_translate(mat4(1.0f)),
	  v_rotate(mat4(1.0f))
{
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	initCube();
	initCoord();
	model_to_view = createView();
}

//----------------------------------------------------------------------------------------
/*
	initialization
*/

void A2::initCoord()
{
	int cnt = 0;
	// x
	coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	coord_verts[cnt++] = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// y
	coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	coord_verts[cnt++] = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	// z
	coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	coord_verts[cnt++] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

void A2::initCube()
{
	int cnt = 0;
	// back 4 lines
	// down
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, -1.0f, -1.0f, 1.0f);
	// right
	cube_verts[cnt++] = vec4(1.0f, 1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, -1.0f, -1.0f, 1.0f);
	// left
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	// up
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, 1.0f, -1.0f, 1.0f);

	// front 4 lines
	// down
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, -1.0f, 1.0f, 1.0f);
	// right
	cube_verts[cnt++] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, -1.0f, 1.0f, 1.0f);
	// left
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	// up
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// left 2 lines
	// down
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	// up
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	// right 2 lines
	// down
	cube_verts[cnt++] = vec4(1.0f, -1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, -1.0f, 1.0f, 1.0f);
	// up
	cube_verts[cnt++] = vec4(1.0f, 1.0f, -1.0f, 1.0f);
	cube_verts[cnt++] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2022
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation("colour");
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
					 GL_DYNAMIC_DRAW);

		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
					 GL_DYNAMIC_DRAW);

		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation("position");
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation("colour");
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour(
	const glm::vec3 &colour)
{
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
	const glm::vec2 &V0, // Line Start (NDC coordinate)
	const glm::vec2 &V1	 // Line End (NDC coordinate)
)
{

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//---------------------------------------------------------------------------------------
/*
	TRANSFORMATION - MODEL
*/ 

void A2::rotate_model(double x_mov) {
	float theta = x_mov * 0.01f;
	
	if (axis.x) {
		mat4 rotate(1.0f);

		rotate[1].y = cos(theta);
		rotate[1].z = sin(theta);
		rotate[2].y = -sin(theta);
		rotate[2].z = cos(theta);

		m_rotate *= rotate;
	}

	if (axis.y) {
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].z = -sin(theta);
		rotate[2].x = sin(theta);
		rotate[2].z = cos(theta);

		m_rotate *= rotate;
	}

	if (axis.z) {
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].y = sin(theta);
		rotate[1].x = -sin(theta);
		rotate[1].y = cos(theta);

		m_rotate *= rotate;
	}


}

void A2::translate_model(double x_mov) {
	double factor = 0.05f;
	mat4 translate(1.0f);

	if (axis.x) {
		translate[3].x = x_mov * factor;
	}

	if (axis.y) {
		translate[3].y = x_mov * factor;
	}

	if (axis.z) {
		translate[3].z = x_mov * factor;
	}

	m_translate *= translate;
}

void A2::scale_model(double x_mov) {
	double factor = 0.05f;
	mat4 translate(1.0f);

	double min_scale;
	1.0f + x_mov * factor > 0 ? min_scale = 1.0f + x_mov * factor : min_scale = 1.0f;

	if (axis.x) {
		translate[0].x *= min_scale;
	}

	if (axis.y) {
		translate[1].y = min_scale;
	}

	if (axis.z) {
		translate[2].z = min_scale;
	}

	m_translate *= translate;
}

/*
	TRANSFORMATION - VIEW
*/

void A2::rotate_view(double x_mov) {
	float theta = x_mov * 0.01f;
	
	if (axis.x) {
		mat4 rotate(1.0f);

		rotate[1].y = cos(theta);
		rotate[1].z = sin(theta);
		rotate[2].y = -sin(theta);
		rotate[2].z = cos(theta);

		v_rotate *= inverse(rotate);
	}

	if (axis.y) {
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].z = -sin(theta);
		rotate[2].x = sin(theta);
		rotate[2].z = cos(theta);

		v_rotate *= inverse(rotate);
	}

	if (axis.z) {
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].y = sin(theta);
		rotate[1].x = -sin(theta);
		rotate[1].y = cos(theta);

		v_rotate *= inverse(rotate);
	}
}

void A2::translate_view(double x_mov) {
	float factor = 0.05f;
	mat4 translate(1.0f);

	if (axis.x) {
		translate[3].x = x_mov * factor;
	}

	if (axis.y) {
		translate[3].y = x_mov * factor;
	}

	if (axis.z) {
		translate[3].z = x_mov * factor;
	}

	m_translate *= inverse(translate);
}

// TODO: modify this
glm::mat4 A2::createView()
{
	glm::vec3 lookat(0.0f, 0.0f, -1.0f);
	glm::vec3 lookfrom(0.0f, 0.0f, 10.0f);
	glm::vec3 up(lookfrom.x, lookfrom.y + 1.0f, lookfrom.z);

	glm::vec3 z((lookat - lookfrom) / glm::distance(lookat, lookfrom));
	glm::vec3 x(glm::cross(z, up) / glm::distance(glm::cross(z, up), glm::vec3(0.0f)));
	glm::vec3 y(glm::cross(x, z));

	glm::mat4 R{
		vec4(x, 0),
		vec4(y, 0),
		vec4(z, 0),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
	};
	R = glm::transpose(R);
	glm::mat4 T(1.0f);
	T[3].z = -lookfrom.z;
	return R * T;
}

//----------------------------------------------------------------------------------------
vec4 A2::point_transformation(vec4 &point, TRANS trans)
{
	vec4 new_pt;
	if (trans == TRANS::MODEL) {
		new_pt = v_rotate * v_translate * model_to_view * m_translate * m_rotate * m_scale * point;
	} else if (trans == TRANS::MODEL_FRAME) {
		new_pt = v_rotate * v_translate * model_to_view * m_translate * m_rotate * m_scale * point;
	} else {
		new_pt = v_rotate * v_translate * model_to_view * point;
	}
	
	new_pt /= new_pt.z;
	return new_pt;
}
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	for (int i = 0; i < cube_num; i += 2)
	{
		setLineColour(vec3(0.0f, 0.0f, 0.0f));
		vec4 p_a = point_transformation(cube_verts[i], TRANS::MODEL);
		vec4 p_b = point_transformation(cube_verts[i + 1], TRANS::MODEL);
		drawLine(vec2(p_a.x, p_a.y), vec2(p_b.x, p_b.y));
	}


	// Model frame
	for (int i = 0; i < coord_num; i += 2)
	{
		setLineColour(vec3(1.0f, 0.7f, 0.8f));
		vec4 p_a = point_transformation(coord_verts[i], TRANS::MODEL_FRAME);
		vec4 p_b = point_transformation(coord_verts[i + 1], TRANS::MODEL_FRAME);
		drawLine(vec2(p_a.x, p_a.y), vec2(p_b.x, p_b.y));
	}

	// View frame
	for (int i = 0; i < coord_num; i += 2)
	{
		setLineColour(vec3(0.2f, 1.0f, 1.0f));
		vec4 p_a = point_transformation(coord_verts[i], TRANS::VIEW_FRAME);
		vec4 p_b = point_transformation(coord_verts[i + 1], TRANS::VIEW_FRAME);
		drawLine(vec2(p_a.x, p_a.y), vec2(p_b.x, p_b.y));
	}
	// // Draw outer square:
	// setLineColour(vec3(1.0f, 0.7f, 0.8f));
	// drawLine(vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
	// drawLine(vec2(0.5f, -0.5f), vec2(0.5f, 0.5f));
	// drawLine(vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f));
	// drawLine(vec2(-0.5f, 0.5f), vec2(-0.5f, -0.5f));

	// // Draw inner square:
	// setLineColour(vec3(0.2f, 1.0f, 1.0f));
	// drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	// drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	// drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	// drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
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

	ImGui::PushID(1);
	ImGui::RadioButton("Rotate View (O)", &current_widget, MODE::ROTATE_VIEW);
	ImGui::RadioButton("Translate View (E)", &current_widget, MODE::TRANSLATE_VIEW);
	ImGui::RadioButton("Perspective (P)", &current_widget, MODE::PERSPECTIVE);
	ImGui::RadioButton("Rotate Model (R)", &current_widget, MODE::ROTATE_MODEL);
	ImGui::RadioButton("Translate Model (T)", &current_widget, MODE::TRANSLATE_MODEL);
	ImGui::RadioButton("Scale Model (S)", &current_widget, MODE::SCALE_MODEL);
	ImGui::RadioButton("Viewport (V)", &current_widget, MODE::VIEWPORT);
	ImGui::PopID();

	// Create Button, and check if it was clicked:
	if (ImGui::Button("Reset (A)"))
	{
		
	}

	if (ImGui::Button("Quit Application"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos()
{

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
						m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
						m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
	glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent(
	int entered)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent(
	double xPos,
	double yPos)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.

		if (m_mouseButtonActive) {
			double move_distance = xPos - prev_mouse_x;
			switch(current_widget) {
				case MODE::ROTATE_VIEW:
					rotate_view(move_distance);
					break;
				case MODE::TRANSLATE_VIEW:
					translate_view(move_distance);
					break;
				case MODE::PERSPECTIVE:
					break;
				case MODE::ROTATE_MODEL:
					rotate_model(move_distance);
					break;
				case MODE::TRANSLATE_MODEL:
					translate_model(move_distance);
					break;
				case MODE::SCALE_MODEL:
					scale_model(move_distance);
					break;
				case MODE::VIEWPORT:
					break;
				default:
					break;
			}
		}
		prev_mouse_x = xPos;
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent(
	int button,
	int actions,
	int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (actions == GLFW_PRESS // && (
		// 	button == GLFW_MOUSE_BUTTON_LEFT ||
		// 	button == GLFW_MOUSE_BUTTON_MIDDLE ||
		// 	button == GLFW_MOUSE_BUTTON_RIGHT )
		)
		{
			// switch (button) {
			// 	case GLFW_MOUSE_BUTTON_LEFT:
			// 		axis = AXIS::X;
			// 		break;
			// 	case GLFW_MOUSE_BUTTON_MIDDLE:
			// 		axis = AXIS::Y;
			// 		break;
			// 	case GLFW_MOUSE_BUTTON_RIGHT:
			// 		axis = AXIS::Z;
			// 		break;
			// 	default:
			// 		break;
			// }
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				axis.x = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				axis.y = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				axis.z = true;
			}
			m_mouseButtonActive = true;
			eventHandled = true;
		}
	}

	if (actions == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
				axis.x = false;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				axis.y = false;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				axis.z = false;
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
bool A2::mouseScrollEvent(
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
bool A2::windowResizeEvent(
	int width,
	int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent(
	int key,
	int action,
	int mods)
{
	bool eventHandled(false);

	if (action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_O:
				current_widget = MODE::ROTATE_VIEW;
				eventHandled = true;
				break;
			case GLFW_KEY_E:
				current_widget = MODE::TRANSLATE_VIEW;
				eventHandled = true;
				break;
			case GLFW_KEY_P:
				current_widget = MODE::PERSPECTIVE;
				eventHandled = true;
				break;
			case GLFW_KEY_R:
				current_widget = MODE::ROTATE_MODEL;
				eventHandled = true;
				break;
			case GLFW_KEY_T:
				current_widget = MODE::TRANSLATE_MODEL;
				eventHandled = true;
				break;
			case GLFW_KEY_S:
				current_widget = MODE::SCALE_MODEL;
				eventHandled = true;
				break;
			case GLFW_KEY_V:
				current_widget = MODE::VIEWPORT;
				eventHandled = true;
				break;
			case GLFW_KEY_A:
			// TODO:: fill in for reset
				break;
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(m_window, GL_TRUE);
				break;
			default:
				break;
		}
	}

	return eventHandled;
}
