#include "BlenderNode.hpp"

using namespace glm;

BlenderNode::BlenderNode( const std::string & name, Primitive *prim )
    : SceneNode(name)
    , m_primitive(prim)
    , m_mat_lib(nullptr)
{
    m_nodeType = NodeType::BlenderNode;
}

void BlenderNode::setMaterialLib(MaterialLib * material_lib)
{
    m_mat_lib = material_lib;
}

bool BlenderNode::intersected(Ray &ray, float tmin, HitRecord &rec)
{
	// std::cout << "blender intersect" << std::endl;
    HitRecord tempRec;
	vec4 transOrig = invtrans * vec4(ray.getOrigin(), 1.0);
	vec4 transDir = invtrans * vec4(ray.getDirection(), 0.0);
	Ray transRay = Ray(vec3(transOrig.x, transOrig.y, transOrig.z),
	vec3(transDir.x, transDir.y, transDir.z));

	bool isIntersected = m_primitive->intersected(transRay, tmin, rec.t, tempRec);
	// std::cout << "blender back" << std::endl;

	if (isIntersected) {
		rec.t = tempRec.t;
		rec.p = tempRec.p;
		rec.normal = tempRec.normal;

        // TODO:: fix
		// rec.material = m_material;
		// std::cout << "Hit mat: "<< tempRec.mat_name << std::endl;
		rec.material =  m_mat_lib->get_mat(tempRec.mat_name);

		rec.uv = tempRec.uv;
		// rec.texture = m_mat_lib;
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