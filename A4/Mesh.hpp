// Termm--Fall 2023

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
#define RENDER_BOUNDING_VOLUMES

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::string& fname );
  Mesh(const glm::vec3 &m_pos, double m_size);

  virtual ~Mesh();

  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	std::string fname;

	#ifdef RENDER_BOUNDING_VOLUMES
		glm::vec3 min_vec;
		glm::vec3 max_vec;
		Primitive *nh_box;
	#endif

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
