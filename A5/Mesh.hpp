// Termm--Fall 2023

#pragma once

#include <vector>
#include <iosfwd>
#include <string>
#include <fstream>
#include "HitRecord.hpp"
#include "Triangle.hpp"

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES



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

  double getArea();
  glm::vec3 getRandomPoint(int rand_idx=-1);
  void getRandomPointAndDirection(glm::mat4 &trans, glm::mat4 &t_invtrans, glm::vec3 &pt, glm::vec3 &direction);
  
  std::string mat_name;

  bool isLight=false;
  bool useBary=true;
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<MeshTriangle> m_faces;
	std::vector<glm::vec2> m_uv_coords;
	std::vector<glm::vec3> m_normals;
	
	// #ifdef RENDER_BOUNDING_VOLUMES
		glm::vec3 min_vec;
		glm::vec3 max_vec;
		Primitive *nh_box;
	// #endif

	bool hasVt=false;
	bool hasVn=false;

  std::string fname;

	bool rayTriangleIntersect(
    const glm::vec3 &orig, const glm::vec3 &dir,
    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
    float &t, float &u, float &v);
	

	void readObjFile(std::ifstream& ifs, size_t &pre_idx_v, size_t &pre_idx_vn, size_t &pre_idx_vt, bool check_o);

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
