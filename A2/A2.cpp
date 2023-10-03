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

	  // projection
	  fov(30.0f),
	  near(1.0f),
	  far(50.0f),

	  // viewport
	  viewport_activated(false),

	  // UI
	  current_widget(0),

	  axis{false, false, false},

	  // mouse
	  m_mouseButtonActive(false),
	  prev_mouse_x(0.0f),

	  // matrix
	  m_transform(mat4(1.0f)),
	  m_scale(mat4(1.0f)),
	  v_transform(mat4(1.0f))
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
	initWorldCoord();
	initModelCoord();

	initViewport();
	initProj();
	initView();
}

void A2::reset()
{
	// projection
	fov = 30.0f;
	near = 1.0f;
	far = 50.0f;

	// UI
	current_widget = 0;

	// matrix
	m_transform = mat4(1.0f);
	m_scale = mat4(1.0f);
	v_transform = mat4(1.0f);

	initView();
	initViewport();
	initProj();
}

//----------------------------------------------------------------------------------------
/*
	initialization
*/

void A2::initViewport()
{
	vp_x1 = 0.05 * m_windowWidth;
	vp_y1 = 0.05 * m_windowHeight;
	vp_x2 = 0.95 * m_windowWidth;
	vp_y2 = 0.95 * m_windowHeight;
}

void A2::initWorldCoord()
{
	int cnt = 0;
	// x
	w_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	w_coord_verts[cnt++] = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// y
	w_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	w_coord_verts[cnt++] = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	// z
	w_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	w_coord_verts[cnt++] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

void A2::initModelCoord()
{
	int cnt = 0;
	// x
	m_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_coord_verts[cnt++] = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// y
	m_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_coord_verts[cnt++] = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	// z
	m_coord_verts[cnt++] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_coord_verts[cnt++] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
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

void A2::rotate_model(double x_mov)
{
	float theta = x_mov * 0.01f;

	if (axis.x)
	{
		mat4 rotate(1.0f);

		rotate[1].y = cos(theta);
		rotate[1].z = sin(theta);
		rotate[2].y = -sin(theta);
		rotate[2].z = cos(theta);

		m_transform *= rotate;
	}

	if (axis.y)
	{
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].z = -sin(theta);
		rotate[2].x = sin(theta);
		rotate[2].z = cos(theta);

		m_transform *= rotate;
	}

	if (axis.z)
	{
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].y = sin(theta);
		rotate[1].x = -sin(theta);
		rotate[1].y = cos(theta);

		m_transform *= rotate;
	}
}

void A2::translate_model(double x_mov)
{
	double factor = 0.05f;
	mat4 translate(1.0f);

	if (axis.x)
	{
		translate[3].x = x_mov * factor;
	}

	if (axis.y)
	{
		translate[3].y = x_mov * factor;
	}

	if (axis.z)
	{
		translate[3].z = x_mov * factor;
	}

	m_transform *= translate;
}

void A2::scale_model(double x_mov)
{
	double factor = 0.05f;
	mat4 scale(1.0f);

	double min_scale;
	1.0f + x_mov *factor > 0 ? min_scale = 1.0f + x_mov *factor : min_scale = 1.0f;

	if (axis.x)
	{
		scale[0].x *= min_scale;
	}

	if (axis.y)
	{
		scale[1].y = min_scale;
	}

	if (axis.z)
	{
		scale[2].z = min_scale;
	}

	m_scale *= scale;
}

/*
	TRANSFORMATION - VIEW
*/

void A2::rotate_view(double x_mov)
{
	float theta = x_mov * 0.01f;

	if (axis.x)
	{
		mat4 rotate(1.0f);

		rotate[1].y = cos(theta);
		rotate[1].z = sin(theta);
		rotate[2].y = -sin(theta);
		rotate[2].z = cos(theta);

		v_transform = inverse(rotate) * v_transform;
	}

	if (axis.y)
	{
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].z = -sin(theta);
		rotate[2].x = sin(theta);
		rotate[2].z = cos(theta);

		v_transform = inverse(rotate) * v_transform;
	}

	if (axis.z)
	{
		mat4 rotate(1.0f);

		rotate[0].x = cos(theta);
		rotate[0].y = sin(theta);
		rotate[1].x = -sin(theta);
		rotate[1].y = cos(theta);

		v_transform = inverse(rotate) * v_transform;
	}
}

void A2::translate_view(double x_mov)
{
	float factor = 0.05f;
	mat4 translate(1.0f);

	if (axis.x)
	{
		translate[3].x = x_mov * factor;
	}

	if (axis.y)
	{
		translate[3].y = x_mov * factor;
	}

	if (axis.z)
	{
		translate[3].z = x_mov * factor;
	}

	v_transform = inverse(translate) * v_transform;
}

/*
----------------------------------------------------------------------
*/

void A2::change_fov(double x_mov)
{
	float factor = 0.05f;
	fov += 0.05f * x_mov;

	if (fov > 160) {
		fov = 160;
	} else if (fov < 5) {
		fov = 5;
	} 
}

void A2::perspective(double x_mov)
{
	float factor = 0.01f;
	if (axis.x)
	{
		change_fov(x_mov);
	}

	if (axis.y)
	{
		near += factor * x_mov;
		near = (near <= 0.1f ? 0.1f : near);
		near = (near >= far - 0.1f ? far - 0.1f : near);
	}

	if (axis.z)
	{
		far = (far + factor * x_mov <= near + 0.1f
				? near + 0.1f : far + factor * x_mov
				);
	}

	initProj();

}


void A2::initView()
{

	glm::vec3 lookat(0.0f, 0.0f, -1.0f);
	glm::vec3 cam_pos(0.0f, 0.0f, 5.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	glm::vec3 z((lookat - cam_pos) / glm::distance(lookat, cam_pos)); // using pts to get normalized
	glm::vec3 x(glm::cross(z, up) / glm::distance(glm::cross(z, up), glm::vec3(0.0f)));
	glm::vec3 y(glm::cross(x, z));

	glm::mat4 R{
		vec4(x, 0),
		vec4(y, 0),
		vec4(z, 0),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
	};

	R = transpose(R);

	glm::mat4 T(1.0f);
	T[3].z = -cam_pos.z;
	model_to_view = R * T;
}

void A2::initProj()
{
	float aspect = m_windowWidth / m_windowHeight;
	float cot_fov = cos(radians(fov)) / sin(radians(fov));

	proj = transpose(mat4{
		cot_fov / aspect,
		0,
		0,
		0,

		0,
		cot_fov,
		0,
		0,

		0,
		0,
		(far + near) / (far - near),
		-2 * far * near / (far - near),

		0,
		0,
		1,
		0,
	});
}

//----------------------------------------------------------------------------------------
//

bool A2::clip_helper(vec4 &A, vec4 &B, float wecA, float wecB)
{
	if (wecA < 0 && wecB < 0)
	{
		return false;
	}

	if (wecA >= 0 && wecB >= 0)
	{
		return true;
	}
	float t = wecA / (wecA - wecB);
	if (wecA < 0)
	{
		A = A + t * (B - A);
	}
	else
	{
		B = A + t * (B - A);
	}
	return true;
}

bool A2::clip(vec4 &A, vec4 &B)
{
	float wecA, wecB;

	// left
	wecA = A.w + A.x;
	wecB = B.w + B.x;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	// right
	wecA = A.w - A.x;
	wecB = B.w - B.x;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	wecA = A.w + A.y;
	wecB = B.w + B.y;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	wecA = A.w - A.y;
	wecB = B.w - B.y;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	wecA = A.w + A.z;
	wecB = B.w + B.z;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	wecA = A.w - A.z;
	wecB = B.w - B.z;
	if (!clip_helper(A, B, wecA, wecB))
		return false;

	return true;
}

vec4 A2::point_transformation(vec4 &point, TRANS trans)
{
	vec4 new_pt;
	if (trans == TRANS::MODEL)
	{
		new_pt = proj * v_transform * model_to_view * m_transform * m_scale * point;
	}
	else if (trans == TRANS::MODEL_FRAME)
	{
		new_pt = proj * v_transform * model_to_view * m_transform * point;
	}
	else
	{
		new_pt = proj * v_transform * model_to_view * point;
	}

	return new_pt;
}

/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// view port
	float vp_left = (vp_x1 < vp_x2 ? vp_x1 : vp_x2) / (m_windowWidth / 2) - 1;
	float vp_right = (vp_x1 < vp_x2 ? vp_x2 : vp_x1) / (m_windowWidth / 2) - 1;
	float vp_bottom = 1 - (vp_y1 < vp_y2 ? vp_y1 : vp_y2) / (m_windowHeight / 2);
	float vp_top = 1 - (vp_y1 < vp_y2 ? vp_y2 : vp_y1) / (m_windowHeight / 2);

	float width_ratio = (vp_right - vp_left) / 2;
	float height_ratio = (vp_bottom - vp_top) / 2;

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	for (int i = 0; i < cube_num; i += 2)
	{
		setLineColour(vec3(0.7f, 1.0f, 0.7f));
		vec4 p_a = point_transformation(cube_verts[i], TRANS::MODEL);
		vec4 p_b = point_transformation(cube_verts[i + 1], TRANS::MODEL);

		if (clip(p_a, p_b))
		{
			p_a /= p_a.w;
			p_b /= p_b.w;
			drawLine(vec2(width_ratio * (p_a.x + 1.0f) + vp_left, height_ratio * (p_a.y + 1.0f) + vp_top),
					 vec2(width_ratio * (p_b.x + 1.0f) + vp_left, height_ratio * (p_b.y + 1.0f) + vp_top));
		}
	}

	// Model frame
	for (int i = 0; i < coord_num; i += 2)
	{
		setLineColour(vec3(1.0f, 0.7f, 0.8f));
		vec4 p_a = point_transformation(m_coord_verts[i], TRANS::MODEL_FRAME);
		vec4 p_b = point_transformation(m_coord_verts[i + 1], TRANS::MODEL_FRAME);

		if (clip(p_a, p_b))
		{
			p_a /= p_a.w;
			p_b /= p_b.w;
			drawLine(vec2(width_ratio * (p_a.x + 1.0f) + vp_left, height_ratio * (p_a.y + 1.0f) + vp_top),
					 vec2(width_ratio * (p_b.x + 1.0f) + vp_left, height_ratio * (p_b.y + 1.0f) + vp_top));
		}
	}

	// World frame
	for (int i = 0; i < coord_num; i += 2)
	{
		setLineColour(vec3(0.0f, 0.1f, 1.0f));
		vec4 p_a = point_transformation(w_coord_verts[i], TRANS::WORLD_FRAME);
		vec4 p_b = point_transformation(w_coord_verts[i + 1], TRANS::WORLD_FRAME);

		if (clip(p_a, p_b))
		{
			p_a /= p_a.w;
			p_b /= p_b.w;
			drawLine(vec2(width_ratio * (p_a.x + 1.0f) + vp_left, height_ratio * (p_a.y + 1.0f) + vp_top),
					 vec2(width_ratio * (p_b.x + 1.0f) + vp_left, height_ratio * (p_b.y + 1.0f) + vp_top));
		}
	}

	// viewport
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	// bottom
	drawLine(vec2(vp_left, vp_bottom), vec2(vp_right, vp_bottom));
	// top
	drawLine(vec2(vp_left, vp_top), vec2(vp_right, vp_top));
	// left
	drawLine(vec2(vp_left, vp_bottom), vec2(vp_left, vp_top));
	// right
	drawLine(vec2(vp_right, vp_bottom), vec2(vp_right, vp_top));
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
		reset();
	}

	if (ImGui::Button("Quit Application (Q)"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Near: %.1f", near);
	ImGui::SameLine();
	ImGui::Text("Far: %.1f", far);
	ImGui::Text("FOV: %.1f", fov);

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
		if (viewport_activated)
		{
			vp_x1 = xPos;
			vp_y1 = yPos;
			prev_mouse_x = xPos;
			viewport_activated = false;
		}

		if (m_mouseButtonActive)
		{
			double move_distance = xPos - prev_mouse_x;
			switch (current_widget)
			{
			case MODE::ROTATE_VIEW:
				rotate_view(move_distance);
				break;
			case MODE::TRANSLATE_VIEW:
				translate_view(move_distance);
				break;
			case MODE::PERSPECTIVE:
				perspective(move_distance);
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
				vp_x2 = clamp((float)xPos, 0.0f, (float)m_windowWidth);
				vp_y2 = clamp((float)yPos, 0.0f, (float)m_windowHeight);
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
		if (actions == GLFW_PRESS)
		{
			if (current_widget == MODE::VIEWPORT)
			{
				viewport_activated = true;
			}

			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				axis.x = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				axis.y = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				axis.z = true;
			}
			m_mouseButtonActive = true;
			eventHandled = true;
		}
	}

	if (actions == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			axis.x = false;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			axis.y = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
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

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
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
			reset();
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
