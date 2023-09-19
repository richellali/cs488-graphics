// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();
	void initAvatar();
	void drawCube();

	void reset();
	void digMaze();
	void removeCube(float x, float z);

	void assignColour(float src[3], float dst[3]);

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Walls related 
	int block_size;
	float block_col[3];
	GLuint cube_vao;
	GLuint cube_vbo;

	// maze
	Maze m;
	bool maze_digged;

	float floor_col[3];

	// avator
	float avatar_col[3];
	GLuint avatar_vao;
	GLuint avatar_vbo;
	glm::vec3 avatar_pos;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[3];

	int current_col;
	int current_widget;

	// Rotation
	bool m_mouseButtonActive;

	// Shape rotation angle in radians.
	float m_shape_rotation;
	double prev_mouse_x;
	glm::vec2 m_mouse_GL_coordinate;

	// Scale
	float m_shape_size;

	// key input
	bool shift_holding;
};
