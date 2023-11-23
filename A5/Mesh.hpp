// Termm--Fall 2023

#pragma once

#include <vector>
#include <iosfwd>
#include <string>
#include <fstream>
#include "HitRecord.hpp"

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	size_t uv1;
	size_t uv2;
	size_t uv3;

	size_t n1;
	size_t n2;
	size_t n3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
		, uv1( 0 )
		, uv2( 0 )
		, uv3( 0 )
		, n1( 0 )
		, n2( 0 )
		, n3( 0 )
	{}

	Triangle( 
		size_t pv1, size_t pv2, size_t pv3,
		size_t tv1, size_t tv2, size_t tv3,
		size_t nv1, size_t nv2, size_t nv3
	 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
		, uv1( tv1 )
		, uv2( tv2 )
		, uv3( tv3 )
		, n1( nv1 )
		, n2( nv2 )
		, n3( nv3 )
	{}

	Triangle(size_t v[3], size_t t[3], size_t n[3])
		: v1( v[0] )
		, v2( v[1] )
		, v3( v[2] )
		, uv1( t[0] )
		, uv2( t[1] )
		, uv3( t[2] )
		, n1( n[0] )
		, n2( n[1] )
		, n3( n[2] )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::string &fname);
  Mesh(const std::string &fname, std::ifstream &ifs, 
  	size_t &pre_idx_v, size_t &pre_idx_vn, size_t &pre_idx_vt);
  Mesh(const glm::vec3 &m_pos, glm::vec3 &m_size);

  virtual ~Mesh();

  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
  void get_uv(HitRecord &rec);
  
  std::string mat_name;
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	std::vector<glm::vec2> m_uv_coords;
	std::vector<glm::vec3> m_normals;
	std::string fname;

	// #ifdef RENDER_BOUNDING_VOLUMES
		glm::vec3 min_vec;
		glm::vec3 max_vec;
		Primitive *nh_box;
	// #endif

	bool hasVt=false;
	bool hasVn=false;

	void readObjFile(std::ifstream& ifs, size_t &pre_idx_v, size_t &pre_idx_vn, size_t &pre_idx_vt);

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
