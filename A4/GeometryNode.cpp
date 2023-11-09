// Termm--Fall 2023

#include "GeometryNode.hpp"
#include <glm/glm.hpp> 
#include <iostream>

using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat , Texture *tex)
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
	, m_texture(tex)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

void GeometryNode::setTexture( Texture *texture )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	std::cout << texture->fname << std::endl;
	m_texture = texture;
}

bool GeometryNode::intersected(Ray &ray, float tmin, HitRecord &rec) {
	HitRecord tempRec;

	vec4 transOrig = invtrans * vec4(ray.getOrigin(), 1.0);
	vec4 transDir = invtrans * vec4(ray.getDirection(), 0.0);
	Ray transRay = Ray(vec3(transOrig.x, transOrig.y, transOrig.z),
	vec3(transDir.x, transDir.y, transDir.z));

	bool isIntersected = m_primitive->intersected(transRay, tmin, rec.t, tempRec);

	if (isIntersected) {
		rec.t = tempRec.t;
		rec.p = tempRec.p;
		rec.normal = tempRec.normal;
		rec.material = m_material;

		rec.u = tempRec.u;
		rec.v = tempRec.v;
		rec.texture = m_texture;
	}

		
	for(SceneNode * child : children) {
		if (child->intersected(transRay, tmin, rec)) {
			isIntersected = true;
		}
	}

	if (isIntersected) {
		vec4 transNormal = transpose(invtrans) * vec4(rec.normal, 0.0f);
		vec4 transPoint = trans * vec4(rec.p, 1.0f);

		rec.normal = vec3(transNormal.x, transNormal.y, transNormal.z);
		rec.p = vec3(transPoint.x, transPoint.y, transPoint.z);
	}

	return isIntersected;
}
