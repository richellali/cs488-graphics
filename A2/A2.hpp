// Termm--Fall 2023

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>


#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;
enum MODE {
	ROTATE_VIEW = 0,
	TRANSLATE_VIEW = 1,
	PERSPECTIVE = 2,
	ROTATE_MODEL = 3,
	TRANSLATE_MODEL = 4,
	SCALE_MODEL = 5,
	VIEWPORT = 6,
};

enum TRANS {
	MODEL = 0,
	MODEL_FRAME = 1,
	WORLD_FRAME = 2,
};

struct Axis{
		bool x;
		bool y;
		bool z;
	};


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void reset();

	// UI
	int current_widget;

	// cubes
	glm::vec4 cube_verts[24];
	int cube_num;
	void initCube();

	// coordinates
	glm::vec4 w_coord_verts[6];
	glm::vec4 m_coord_verts[6];
	int coord_num;
	void initWorldCoord();
	void initModelCoord();

	// viewport
	bool viewport_activated;
	// float init_view_lb_x;
	// float init_view_lb_y;
	// float final_view_rt_x;
 	// float final_view_rt_y;

	float vp_x1;
	float vp_y1;
	float vp_x2;
 	float vp_y2;

	void initViewport();

	// mouse
	Axis axis;
	bool m_mouseButtonActive;
	double prev_mouse_x;

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	// matrix 
	// model
	glm::mat4 m_transform;
	glm::mat4 m_scale;

	// view
	glm::mat4 model_to_view;
	glm::mat4 v_transform;

	// projection
	float fov;
	float near;
	float far;
	glm::mat4 proj;
	void initProj();

	// perpective
	void change_fov(double x_mov);
	void perspective(double x_mov);

	bool clip_helper(glm::vec4 &A, glm::vec4 &B, float wecA, float wecB);
	bool clip(glm::vec4 &A, glm::vec4 &B);

	// transformation func
	void initView();
	glm::vec4 point_transformation(glm::vec4 &point, TRANS trans);

	void rotate_view(double x_mov);
	void translate_view(double x_mov);
	void rotate_model(double x_mov);
	void translate_model(double x_mov);
	void scale_model(double x_mov);
	

};
