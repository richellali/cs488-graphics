// Termm-Fall 2023

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};
	float xAngle;
	float yAngle;

	JointRange m_joint_x, m_joint_y;

	void rotate(char axis, float angle);

	void setXAngle(float angle);
	void setYAngle(float angle);

	bool isNotRotatable();

	float restrictX(float angleChanges);
	float restrictY(float angleChanges);

	void setXRotation(float angle);
	void setYRotation(float angle);
};
