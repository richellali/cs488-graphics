// Termm--Fall 2023

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "HitRecord.hpp"
#include "Light.hpp"

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode,
    BlenderNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    virtual void collect_lights(std::list<AreaLight *> &lights);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;
    std::vector<Light*> lights_vec;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    // Ray Tracing
    virtual bool intersected(Ray &ray, float tmin, HitRecord &rec);

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
