// Termm-Fall 2023

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <memory>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

enum MODE {
	POSITION_ORIEN = 0,
	JOINTS = 1,
};

struct M_Button{
		bool left;
		bool middle;
		bool right;
	};

struct JointStat {
	JointNode *node;
	float xAngle;
	float yAngle;
	glm::mat4 trans;
	bool isSelected;

};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderArcCircle();
	void renderGeometryNode(const SceneNode *node, glm::mat4 modelMatrix);

	//-- Transformation
	void translate_puppet(double x_mov, double y_mov);
	void rotate_puppet(double xPos, double yPos);
	void rotate_joints(double x_mov, double y_mov);

	//-- Picking
	void initPickingMap(SceneNode &node);
	void mouseClickedPicking();
	void selectOrDisselectJoints(unsigned int nodeId, SceneNode &node);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	// -- GUI
	int current_widget;
	bool circle_box;
	bool zBuffer_box;
	bool backfaceCulling_box;
	bool frontfaceCulling_box;

	// -- Mouse Input
	bool m_mouseButtonActive;
	double prev_mouse_x;
	double prev_mouse_y;
	M_Button m_button;

	// -- transformation
	bool areJointsRotated;
	glm::mat4 model_translate;
	glm::mat4 model_rotate;

	// -- picking
	bool do_picking;
	std::map<unsigned int, SceneNode *> pickingMap;
	std::vector<SceneNode*> selectedJoints;

	// -- GUI
	void resetPos();
	void resetOrien();
	void resetJoints();
	void resetAll();

	// -- Undo/Redo
	int curStackIdx;
	int curStackSize;
	std::vector<std::vector<JointStat>> undoRedoStack;

	void applyCurIdxToStack();
	void undoStack();
	void redoStack();
	void addNewStatToStack();
	void createUndoRedoStack(SceneNode* node, int idx);
};
