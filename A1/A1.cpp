// Termm--Fall 2023

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
static const float PI = 3.1415926;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col(0), // ui widget
	  current_widget(0),

	  m(Maze{DIM}),
	  block_size(1),
	  avatar_pos(vec3(0.0f)),
	  maze_digged(false),

	  // rotation
	  prev_mouse_x(double(0)),
	  m_shape_rotation(0.0f),
	  insta_rotation_v(0.0f),
	  m_mouse_GL_coordinate(vec2(0.0f)),

	  // scale
	  m_shape_size(1.0f),

	  // keyboard
	  shift_holding(false)
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;

	block_col[0] = 0.04f;
	block_col[1] = 0.2f;
	block_col[2] = 0.4f;

	floor_col[0] = 0.4f;
	floor_col[1] = 0.7f;
	floor_col[2] = 0.7f;

	avatar_col[0] = 1.0f;
	avatar_col[1] = 1.0f;
	avatar_col[2] = 1.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed = getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers

	// cout << "Random number seed = " << rseed << endl;

	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(
		getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation("P");
	V_uni = m_shader.getUniformLocation("V");
	M_uni = m_shader.getUniformLocation("M");
	col_uni = m_shader.getUniformLocation("colour");

	initGrid();
	initFloor();
	initAvatar();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3(0.0f, 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	proj = glm::perspective(
		glm::radians(30.0f),
		float(m_framebufferWidth) / float(m_framebufferHeight),
		1.0f, 1000.0f);
}

void A1::drawCube()
{
	int sz = m.getBlockNum();
	vec3 verts[3 * 12 * sz];

	int i, j;
	int cnt = 0;

	for (i = 0; i < DIM; i++)
	{
		for (j = 0; j < DIM; j++)
		{
			if (m.getValue(i, j) == 1)
			{
				float x = float(j);
				float z = float(i);
				float y = float(block_size);

				// front
				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, y, z);

				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x, y, z);
				verts[cnt++] = vec3(x + 1.0f, y, z);

				// left
				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x, y, z);
				verts[cnt++] = vec3(x, y, z + 1.0f);

				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x, 0.0f, z + 1.0f);
				verts[cnt++] = vec3(x, y, z + 1.0f);

				// back
				verts[cnt++] = vec3(x, 0.0f, z + 1.0f);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f);

				verts[cnt++] = vec3(x, 0.0f, z + 1.0f),
				verts[cnt++] = vec3(x, y, z + 1.0f),
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f),

				// right
					verts[cnt++] = vec3(x + 1.0f, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, y, z);
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f);

				verts[cnt++] = vec3(x + 1.0f, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f);

				// up
				verts[cnt++] = vec3(x, y, z);
				verts[cnt++] = vec3(x, y, z + 1.0f);
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f);

				verts[cnt++] = vec3(x, y, z);
				verts[cnt++] = vec3(x + 1.0f, y, z);
				verts[cnt++] = vec3(x + 1.0f, y, z + 1.0f);

				// bottom
				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x, 0.0f, z + 1.0f);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);

				verts[cnt++] = vec3(x, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z);
				verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);
			}
		}
	}

	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 12 * sz * sizeof(vec3), verts, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

void A1::initFloor()
{
	int sz = DIM * DIM;
	vec3 verts[3 * 2 * sz];

	int i, j;
	int cnt = 0;

	for (i = 0; i < DIM; i++)
	{
		for (j = 0; j < DIM; j++)
		{
			float x = float(j);
			float z = float(i);
			// bottom
			verts[cnt++] = vec3(x, 0.0f, z);
			verts[cnt++] = vec3(x, 0.0f, z + 1.0f);
			verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);

			verts[cnt++] = vec3(x, 0.0f, z);
			verts[cnt++] = vec3(x + 1.0f, 0.0f, z);
			verts[cnt++] = vec3(x + 1.0f, 0.0f, z + 1.0f);
		}
	}

	glGenVertexArrays(1, &floor_vao);
	glBindVertexArray(floor_vao);

	glGenBuffers(1, &floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sz * sizeof(vec3), verts, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM + 3);

	float *verts = new float[sz];
	size_t ct = 0;
	for (int idx = 0; idx < DIM + 3; ++idx)
	{
		verts[ct] = -1;
		verts[ct + 1] = 0;
		verts[ct + 2] = idx - 1;
		verts[ct + 3] = DIM + 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = idx - 1;
		ct += 6;

		verts[ct] = idx - 1;
		verts[ct + 1] = 0;
		verts[ct + 2] = -1;
		verts[ct + 3] = idx - 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = DIM + 1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_grid_vao);
	glBindVertexArray(m_grid_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_grid_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_grid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float),
				 verts, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete[] verts;

	CHECK_GL_ERRORS;
}

void A1::initAvatar()
{
	// src: http://songho.ca/opengl/gl_sphere.html#:~:text=In%20order%20to%20draw%20the,triangle%20strip%20cannot%20be%20used
	size_t sz = 3 * 2 * 180 * 360;
	vec3 verts[sz];

	float r = 0.5f;

	int i, j;
	int cnt = 0;

	for (i = 0; i < 360; i++)
	{
		for (j = 0; j < 180; j++)
		{
			// theta = 2 * PI * i / 360 =  i * PI / 180
			// phi = PI/2 - PI * j / 180 = (90 - j) * PI / 180
			verts[cnt++] = vec3(r + r * cos((90 - j) * PI / 180) * cos(i * PI / 180),
								r * sin((90 - j) * PI / 180),
								r + r * cos((90 - j) * PI / 180) * sin(i * PI / 180));
			verts[cnt++] = vec3(r + r * cos((90 - j - 1) * PI / 180) * cos(i * PI / 180),
								r * sin((90 - j - 1) * PI / 180),
								r + r * cos((90 - j - 1) * PI / 180) * sin(i * PI / 180));
			verts[cnt++] = vec3(r + r * cos((90 - j) * PI / 180) * cos((i + 1) * PI / 180),
								r * sin((90 - j) * PI / 180),
								r + r * cos((90 - j) * PI / 180) * sin((i + 1) * PI / 180));

			verts[cnt++] = vec3(r + r * cos((90 - j - 1) * PI / 180) * cos(i * PI / 180),
								r * sin((90 - j - 1) * PI / 180),
								r + r * cos((90 - j - 1) * PI / 180) * sin(i * PI / 180));
			verts[cnt++] = vec3(r + r * cos((90 - j) * PI / 180) * cos((i + 1) * PI / 180),
								r * sin((90 - j) * PI / 180),
								r + r * cos((90 - j) * PI / 180) * sin((i + 1) * PI / 180));
			verts[cnt++] = vec3(r + r * cos((90 - j - 1) * PI / 180) * cos((i + 1) * PI / 180),
								r * sin((90 - j - 1) * PI / 180),
								r + r * cos((90 - j - 1) * PI / 180) * sin((i + 1) * PI / 180));
		}
	}

	glGenVertexArrays(1, &avatar_vao);
	glBindVertexArray(avatar_vao);

	glGenBuffers(1, &avatar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(vec3), verts, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * GUI function
 */

void A1::assignColour(float src[3], float dst[3])
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

void A1::reset()
{
	// colours
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;

	block_col[0] = 0.04f;
	block_col[1] = 0.2f;
	block_col[2] = 0.4f;

	floor_col[0] = 0.4f;
	floor_col[1] = 0.7f;
	floor_col[2] = 0.7f;

	avatar_col[0] = 1.0f;
	avatar_col[1] = 1.0f;
	avatar_col[2] = 1.0f;

	// GUI
	current_widget = 0;
	current_col = 0;

	// rotation
	m_shape_rotation = 0.0f;
	insta_rotation_v = 0.0f;

	// scale
	m_shape_size = 1.0f;

	// maze
	block_size = 1;
	m.reset();
	maze_digged = false;

	// avatar
	avatar_pos = vec3(0.0f);
}

void A1::digMaze()
{

	m.digMaze();
	m.printMaze();
	std::tuple<float, float> entry = m.getEntry();
	avatar_pos.x = get<1>(entry);
	avatar_pos.z = get<0>(entry);

	drawCube();

	maze_digged = true;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	m_shape_rotation += insta_rotation_v;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
	if (ImGui::Button("Quit Application(Q)"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset(R)"))
	{
		reset();
	}

	ImGui::SameLine();
	if (ImGui::Button("Dig Maze(D)"))
	{
		digMaze();
	}

	// Eventually you'll create multiple colour widgets with
	// radio buttons.  If you use PushID/PopID to give them all
	// unique IDs, then ImGui will be able to keep them separate.
	// This is unnecessary with a single colour selector and
	// radio button, but I'm leaving it in as an example.

	// Prefixing a widget name with "##" keeps it from being
	// displayed.

	// Radio button: 0 - maze blocks, 1 - floor, 2 - avatar
	ImGui::PushID(1);
	ImGui::RadioButton("Block Colour", &current_widget, 0);
	ImGui::RadioButton("Floor Colour", &current_widget, 1);
	ImGui::RadioButton("Avatar Colour", &current_widget, 2);
	ImGui::PopID();

	// check which is the current widget
	switch (current_widget)
	{
	case 0:
		assignColour(block_col, colour);
		break;
	case 1:
		assignColour(floor_col, colour);
		break;
	case 2:
		assignColour(avatar_col, colour);
		break;
	default:
		// cout << "No Radio Button is Selected." << endl;
		break;
	}

	// Set colour to the current widget
	ImGui::ColorEdit3("##Colour", colour);

	// Set colour in the selector to avatar, block floor
	switch (current_widget)
	{
	case 0:
		assignColour(colour, block_col);
		break;
	case 1:
		assignColour(colour, floor_col);
		break;
	case 2:
		assignColour(colour, avatar_col);
		break;
	default:
		// cout << "No Radio Button is Selected." << endl;
		break;
	}

	// For convenience, you can uncomment this to show ImGui's massive
	// demonstration window right in your application.  Very handy for
	// browsing around to get the widget you want.  Then look in
	// shared/imgui/imgui_demo.cpp to see how it's done.
	// if (ImGui::Button("Test Window"))
	// {
	// 	showTestWindow = !showTestWindow;
	// }

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();

	if (showTestWindow)
	{
		ImGui::ShowTestWindow(&showTestWindow);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	vec3 z_axis(0.0f, 1.0f, 0.0f);
	W = glm::scale(W, vec3(m_shape_size));
	W = glm::rotate(W, m_shape_rotation, z_axis);
	W = glm::translate(W, vec3(-float(DIM) / 2.0f, 0, -float(DIM) / 2.0f));

	mat4 center = W;

	m_shader.enable();
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(proj));
	glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

	// Draw the floor
	glBindVertexArray(floor_vao);
	glUniform3f(col_uni, floor_col[0], floor_col[1], floor_col[2]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * DIM * DIM);

	// Just draw the grid for now.
	glBindVertexArray(m_grid_vao);
	glUniform3f(col_uni, 1, 1, 1);
	glDrawArrays(GL_LINES, 0, (3 + DIM) * 4);

	// Draw the cubes
	// Highlight the active square.
	if (maze_digged)
	{
		glBindVertexArray(cube_vao);
		glUniform3f(col_uni, block_col[0], block_col[1], block_col[2]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12 * m.getBlockNum());
	}

	// Draw the Avatar
	glBindVertexArray(avatar_vao);
	glUniform3f(col_uni, avatar_col[0], avatar_col[1], avatar_col[2]);

	W = glm::translate(W, avatar_pos);
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
	glDrawArrays(GL_LINES, 0, 3 * 2 * 180 * 360);
	W = center;

	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent(
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
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);

	m_mouse_GL_coordinate = vec2(
		(2.0f * xPos) / m_windowWidth - 1.0f,
		1.0f - ((2.0f * yPos) / m_windowHeight));

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.

		if (m_mouseButtonActive)
		{
			m_shape_rotation += (xPos - prev_mouse_x) / m_windowWidth / 2 * 2 * PI;
			insta_rotation_v = (xPos - prev_mouse_x) / m_windowWidth / 2 * 2 * PI;
			m_mouseDragging = true;
			eventHandled = true;
		}
		prev_mouse_x = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS)
		{
			m_mouseButtonActive = true;
			m_mouseDragging = false;

			eventHandled = true;
		}
	}
	if (actions == GLFW_RELEASE && m_mouseButtonActive)
	{
		if (!m_mouseDragging)
		{
			insta_rotation_v = 0.0f;
		}
		else
		{
			m_mouseDragging = false;
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
bool A1::mouseScrollEvent(double xOffSet, double yOffSet)
{
	bool eventHandled(false);

	// Zoom in or out.

	// scroll up -> scale up
	if (yOffSet > 0)
	{
		m_shape_size += 0.5f;
		if (m_shape_size > 5)
		{
			m_shape_size = 5;
			// cout << "Maximum Size Reached..." << endl;
		}
		eventHandled = true;
	}
	else
	{
		// scroll down -> scale down
		m_shape_size -= 0.5f;
		if (m_shape_size < 1)
		{
			m_shape_size = 1;
			// cout << "Minimum Size Reached..." << endl;
		}
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Helper to key input events.
 */
void A1::removeCube(float x, float z)
{
	if (0.0f <= x and x < float(DIM) and 0 <= z and z < float(DIM))
	{

		if (m.getValue(int(z), int(x)) == 1)
		{
			m.setValue(int(z), int(x), 0);
			m.decreBlockNum();
			drawCube();
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods)
{
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS)
	{
		// Respond to some key events.
		if (key == GLFW_KEY_SPACE)
		{
			// cout << "space key pressed - growing walls..." << endl;

			block_size += 1;
			drawCube();

			eventHandled = true;
		}

		if (key == GLFW_KEY_BACKSPACE)
		{
			// cout << "backspace key pressed - shrinking walls..." << endl;

			block_size -= 1;

			block_size = block_size >= 1 ? block_size : 1;
			drawCube();

			eventHandled = true;
		}

		if (key == GLFW_KEY_UP)
		{
			// cout << "Up key pressed - going up..." << endl;

			if (avatar_pos.z - 1.0f >= 0.0f)
			{
				if (mods == GLFW_MOD_SHIFT)
				{
					removeCube(avatar_pos.x, avatar_pos.z - 1.0f);
				}
				if (m.getValue(int(avatar_pos.z - 1.0f), int(avatar_pos.x)) == 0)
				{
					avatar_pos.z -= 1.0f;
				}
			}
			else
			{
				// cout << "Limit reached - cannot move up" << endl;
			}

			eventHandled = true;
		}

		if (key == GLFW_KEY_DOWN)
		{
			// cout << "Down key pressed - going down..." << endl;

			if (avatar_pos.z + 1.0f < float(DIM))
			{
				if (mods == GLFW_MOD_SHIFT)
				{
					removeCube(avatar_pos.x, avatar_pos.z + 1.0f);
				}
				if (m.getValue(int(avatar_pos.z + 1.0f), int(avatar_pos.x)) == 0)
				{
					avatar_pos.z += 1.0f;
				}
			}
			else
			{
				// cout << "Limit reached - cannot move down" << endl;
			}

			eventHandled = true;
		}

		if (key == GLFW_KEY_LEFT)
		{
			// cout << "Left key pressed - going left..." << endl;

			if (avatar_pos.x - 1.0f >= 0.0f)
			{
				if (mods == GLFW_MOD_SHIFT)
				{
					removeCube(avatar_pos.x - 1.0f, avatar_pos.z);
				}
				if (m.getValue(int(avatar_pos.z), int(avatar_pos.x - 1.0f)) == 0)
				{
					avatar_pos.x -= 1.0f;
				}
			}
			else
			{
				// cout << "Limit reached - cannot move left" << endl;
			}

			eventHandled = true;
		}

		if (key == GLFW_KEY_RIGHT)
		{
			// cout << "right key pressed - going right..." << endl;

			if (avatar_pos.x + 1.0f < float(DIM))
			{
				if (mods == GLFW_MOD_SHIFT)
				{
					removeCube(avatar_pos.x + 1.0f, avatar_pos.z);
				}
				if (m.getValue(int(avatar_pos.z), int(avatar_pos.x + 1.0f)) == 0)
				{
					avatar_pos.x += 1.0f;
				}
			}
			else
			{
				// cout << "Limit reached - cannot move right" << endl;
			}

			eventHandled = true;
		}

		// HOT KEY
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_R)
		{
			reset();
			eventHandled = true;
		}
		if (key == GLFW_KEY_D)
		{
			digMaze();
			eventHandled = true;
		}
	}

	return eventHandled;
}
