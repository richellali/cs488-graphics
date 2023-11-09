// Termm--Fall 2023

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace glm;

// source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/
// ray-triangle-intersection-geometric-solution.html#:~:text=The%20ray%20can%20intersect%20the,these%20two%20vectors%20is%200).
bool rayTriangleIntersect(
    const vec3 &orig, const vec3 &dir,
    const vec3 &v0, const vec3 &v1, const vec3 &v2,
    float &t)
{
  float eps = 0.000001;
  // compute the plane's normal
  vec3 v0v1 = v1 - v0;
  vec3 v0v2 = v2 - v0;

  vec3 planeNormal = cross(v0v1, v0v2);

  // check if the ray and plane are parallel

  if (abs(dot(planeNormal, dir)) < eps) {
    return false;
  }

  float D = -dot(planeNormal, v0);
  t = - (dot(planeNormal, orig) + D) / dot(planeNormal, dir);
  if (t < 0) return false;

  vec3 p = orig + t * dir;

  // inside outside test
  vec3 C;

  vec3 edge0 = v1 - v0;
  vec3 vp0 = p - v0;
  C = cross(edge0, vp0);
  if (dot(planeNormal, C) < 0) return false;

  vec3 edge1 = v2 - v1;
  vec3 vp1 = p - v1;
  C = cross(edge1, vp1);
  if (dot(planeNormal, C) < 0) return false;

  vec3 edge2 = v0 - v2;
  vec3 vp2 = p - v2;
  C = cross(edge2, vp2);
  if (dot(planeNormal, C) < 0) return false;

  return true;
}

Mesh::Mesh(const std::string &fname)
    : m_vertices(), m_faces(), fname(fname)
{
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::ifstream ifs(fname.c_str());
  while (ifs >> code)
  {
    if (code == "v")
    {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back(glm::vec3(vx, vy, vz));
      #ifdef RENDER_BOUNDING_VOLUMES
      min_vec.x = min(vx, (double)min_vec.x);
      min_vec.y = min(vx, (double)min_vec.y);
      min_vec.z = min(vx, (double)min_vec.z);

		  max_vec.x = max(vx, (double)max_vec.x);
      max_vec.y = max(vx, (double)max_vec.y);
      max_vec.z = max(vx, (double)max_vec.z);
	    #endif
    }
    else if (code == "f")
    {
      ifs >> s1 >> s2 >> s3;
      m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
    }
  }
  #ifdef RENDER_BOUNDING_VOLUMES
  vec3 temp = max_vec - min_vec;
  double max_sz = max(max(temp.x, temp.y), temp.z);
  nh_box = new NonhierBox(min_vec, max_sz);
  #endif
}

Mesh::Mesh(const glm::vec3 &m_pos, double m_size)
    : m_vertices(), m_faces(), fname("cube.obj")
{
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::ifstream ifs(fname.c_str());
  while (ifs >> code)
  {
    if (code == "v")
    {
      ifs >> vx >> vy >> vz;
      glm::vec3 vec = glm::vec3(vx, vy, vz);
      vec *= m_size;
      vec += m_pos;
      m_vertices.push_back(vec);
    }
    else if (code == "f")
    {
      ifs >> s1 >> s2 >> s3;
      m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
    }
  }
}

Mesh::~Mesh()
{
    #ifdef RENDER_BOUNDING_VOLUMES
    if (nh_box) delete nh_box;
    #endif
}

std::ostream &operator<<(std::ostream &out, const Mesh &mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
    const MeshVertex& v = mesh.m_verts[idx];
    out << glm::to_string( v.m_position );
  if( mesh.m_have_norm ) {
      out << " / " << glm::to_string( v.m_normal );
  }
  if( mesh.m_have_uv ) {
      out << " / " << glm::to_string( v.m_uv );
  }
  }

*/
  out << "}";
  return out;
}

bool Mesh::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec)
{
  bool isIntersected = false;
  float tClosest = tmax;

  vec3 normal;

  #ifdef RENDER_BOUNDING_VOLUMES
  HitRecord temp;
  temp.t = std::numeric_limits<float>::max();
   if (nh_box && !nh_box->intersected(ray, tmin, tmax, temp)) {
    return isIntersected;
  }
  #endif

  for (Triangle triangle : m_faces)
  {
    float t;

    if (rayTriangleIntersect(ray.getOrigin(), ray.getDirection(),
                             m_vertices[triangle.v1], m_vertices[triangle.v2], m_vertices[triangle.v3], t) &&
        t < tClosest && t > tmin)
    {
      tClosest = t;
      isIntersected = true;

      normal = cross(m_vertices[triangle.v2]-m_vertices[triangle.v1], m_vertices[triangle.v3]-m_vertices[triangle.v1]);
    }
  }

  if (isIntersected) {
    rec.t = tClosest;
    rec.p = ray.at(rec.t);
    rec.normal = normal;
  }
  
  return isIntersected;
}