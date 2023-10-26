// Termm-Fall 2023

#include "JointNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	xAngle = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	yAngle = init;
}

bool JointNode::isNotRotatable() {
	return m_joint_x.min == m_joint_x.max && m_joint_y.min == m_joint_y.max;
}

void JointNode::setXAngle(float angle) {
	setXRotation(angle-xAngle);
	xAngle = angle;
}

void JointNode::setYAngle(float angle) {
	setYRotation(angle-yAngle);
	yAngle = angle;
}

float JointNode::restrictX(float angleChanges) {
	// std::cout << "angleChanges: " << angleChanges << std::endl;
	// std::cout << "m_joint_x: " << m_joint_x.min << " " << m_joint_x.max << std::endl;
	// std::cout << "xAngle " << xAngle << std::endl;

	if (angleChanges + xAngle < m_joint_x.min){
		angleChanges = m_joint_x.min - xAngle;
		xAngle = m_joint_x.min;

	} else if (angleChanges + xAngle > m_joint_x.max) {
		angleChanges = m_joint_x.max - xAngle;
		xAngle = m_joint_x.max;
	}
	xAngle += angleChanges;
	return angleChanges;
}

float JointNode::restrictY(float angleChanges) {
	// std::cout << "angleChanges: " << angleChanges << std::endl;
	// std::cout << "m_joint_y: " << m_joint_y.min << " " << m_joint_y.max << std::endl;
	// std::cout << "yAngle " << yAngle << std::endl;

	if (angleChanges + yAngle < m_joint_y.min){
		angleChanges = m_joint_y.min - yAngle;
		yAngle = m_joint_y.min;

	} else if (angleChanges + yAngle > m_joint_y.max) {
		angleChanges = m_joint_y.max - yAngle;
		yAngle = m_joint_y.max;
	}

	yAngle += angleChanges;
	return angleChanges;
}

void JointNode::setXRotation(float angle) {
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), vec3(1, 0, 0));
	trans = trans * rot_matrix;
}

void JointNode::setYRotation(float angle) {
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), vec3(0, 1, 0));
	trans = trans * rot_matrix;
}

void JointNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			angle = restrictX(angle);
			if (angle != 0) setXRotation(angle);
			return;
		case 'y':
			angle = restrictY(angle);
			if (angle != 0) setYRotation(angle);
			return;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = trans * rot_matrix;
}


