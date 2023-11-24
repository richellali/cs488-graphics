// Termm--Fall 2023

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr);

	void setMaterial( Material *material );
	// void setTexture(Texture *texture);

	virtual ~GeometryNode();

	Material *m_material;
	Primitive *m_primitive;
	// Texture *m_texture;

	bool intersected(Ray &ray, float tmin, HitRecord &rec) override;
};
