// Termm--Fall 2023

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	invtrans(mat4()),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = glm::inverse(m);
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	set_transform( rot_matrix * trans );
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	set_transform( glm::scale(amount) * trans );
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	set_transform( glm::translate(amount) * trans );
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
		case NodeType::BlenderNode:
			os << "BlenderNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;

	os << "]\n";
	return os;
}

//---------------------------------------------------------------------------------------
bool SceneNode::intersected(Ray &ray, float tmin, HitRecord &rec){
	bool isIntersected = false;

	vec4 transOrig = invtrans * vec4(ray.getOrigin(), 1.0);
	vec4 transDir = invtrans * vec4(ray.getDirection(), 0.0);
	Ray transRay = Ray(vec3(transOrig.x, transOrig.y, transOrig.z),
	vec3(transDir.x, transDir.y, transDir.z));

	for(SceneNode * child : children) {
		if (child->intersected(transRay, tmin, rec)) {
			isIntersected = true;
		}
	}

	if (isIntersected) {
		vec4 transNormal = transpose(invtrans) * vec4(rec.normal, 0.0f);
		vec4 transPoint = trans * vec4(rec.p, 1.0f);
		vec4 transShadingNormal = transpose(invtrans) * vec4(rec.shading_normal, 0.0f);

		rec.normal = vec3(transNormal.x, transNormal.y, transNormal.z);
		rec.p = vec3(transPoint.x, transPoint.y, transPoint.z);
		rec.shading_normal = vec3(transShadingNormal.x, transShadingNormal.y, transShadingNormal.z);
	}


	return isIntersected;
}

void SceneNode::collect_lights(std::list<AreaLight *> &lights){
	for(SceneNode * child : children) {
		child->collect_lights(lights);
	}

	for (auto l : lights)
	{
		l->t_invtrans = transpose(invtrans) * l->t_invtrans;
		l->trans = trans * l->trans;
	}
}