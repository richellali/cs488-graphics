#include "BlenderNode.hpp"

using namespace glm;

BlenderNode::BlenderNode( const std::string & name, BlenderMesh *prim )
    : SceneNode(name)
    , m_b_mesh(prim)
    , m_mat_lib(nullptr)
{
    m_nodeType = NodeType::BlenderNode;
}

void BlenderNode::setMaterialLib(MaterialLib * material_lib)
{
    m_mat_lib = material_lib;
}

BlenderNode::~BlenderNode() {
	if ( m_mat_lib != nullptr) delete m_mat_lib;
	if (m_b_mesh != nullptr) delete m_b_mesh;
}

bool BlenderNode::intersected(Ray &ray, float tmin, HitRecord &rec)
{
	// std::cout << "blender intersect" << std::endl;
    HitRecord tempRec;
	vec4 transOrig = invtrans * vec4(ray.getOrigin(), 1.0);
	vec4 transDir = invtrans * vec4(ray.getDirection(), 0.0);
	Ray transRay = Ray(vec3(transOrig.x, transOrig.y, transOrig.z),
	vec3(transDir.x, transDir.y, transDir.z));

	bool isIntersected = m_b_mesh->intersected(transRay, tmin, rec.t, tempRec);
	// std::cout << "blender back" << std::endl;

	if (isIntersected) {
		rec = tempRec;
		rec.material =  m_mat_lib->get_mat(tempRec.mat_name);
	}

		
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

void BlenderNode::collect_lights(std::list<AreaLight *> &lights)
{
	std::map<std::string, glm::vec3> emissiveNamesAndKe = m_mat_lib->getEmissiveNamesAndKe();
	m_b_mesh->collect_lights(lights, emissiveNamesAndKe);

	SceneNode::collect_lights(lights);

	for (auto l : lights)
	{
		l->t_invtrans = transpose(invtrans) * l->t_invtrans;
		l->trans = trans * l->trans;
	}
}