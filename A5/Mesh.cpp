// Termm--Fall 2023

#include <iostream>
#include <sstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "utils.hpp"

std::string OBJ_DIR = "obj/";

using namespace glm;

// source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/
// ray-triangle-intersection-geometric-solution.html#:~:text=The%20ray%20can%20intersect%20the,these%20two%20vectors%20is%200).
bool Mesh::rayTriangleIntersect(
    const vec3 &orig, const vec3 &dir,
    const vec3 &v0, const vec3 &v1, const vec3 &v2,
    float &t, float &u, float &v)
{
  // std::cout << fname << std::endl;
  float eps = 0.000001;
  // compute the plane's normal
  vec3 v0v1 = v1 - v0;
  vec3 v0v2 = v2 - v0;

  vec3 planeNormal = cross(v0v1, v0v2);

  float denom = dot(planeNormal, planeNormal);

  // check if the ray and plane are parallel
  if (abs(dot(planeNormal, dir)) < eps)
  {
    // if (fname == "smstdodeca.obj") std::cout << "plane" << std::endl;
    return false;
  }

  float D = -dot(planeNormal, v0);
  t = -(dot(planeNormal, orig) + D) / dot(planeNormal, dir);
  if (t < 0) { 
    // if (fname == "smstdodeca.obj") std::cout <<2<< std::endl;
    return false;}

  vec3 p = orig + t * dir;

  // inside outside test
  vec3 C;

  vec3 edge0 = v1 - v0;
  vec3 vp0 = p - v0;
  C = cross(edge0, vp0);
  if (dot(planeNormal, C) < 0){ 
    // if (fname == "smstdodeca.obj") std::cout <<3<< std::endl;
    return false;}

  vec3 edge1 = v2 - v1;
  vec3 vp1 = p - v1;
  C = cross(edge1, vp1);
  if ((u = dot(planeNormal, C)) < 0){ 
    // if (fname == "smstdodeca.obj") std::cout <<4<< std::endl;
    return false;}
  

  vec3 edge2 = v0 - v2;
  vec3 vp2 = p - v2;
  C = cross(edge2, vp2);
  if ((v = dot(planeNormal, C)) < 0) return false;


  u /= denom;
  v /= denom;

  return true;
}

size_t splitF(std::string str, size_t indices[3]) {
  std::stringstream ss(str);
  std::string index_str;
  size_t index;

  size_t res = 0;
  bool own_vt = false;

  while (std::getline(ss, index_str, '/'))
  {
    // std::cout << "index_str: " << index_str << std::endl;
    if (index_str != "") {
      std::stringstream index_ss(index_str);
      index_ss >> index;
      indices[res] = index;

      if (res == 1) own_vt = true;
    } else {
      indices[res] = 0;
    }
    res++;
  }
  if (res == 3 && !own_vt) res++;
  return res;
}

Mesh::Mesh(const std::string &fname)
    : m_vertices(), m_faces(), m_uv_coords(), m_normals(), fname(fname), nh_box(nullptr)
{
  std::ifstream ifs((OBJ_DIR+fname).c_str());
  size_t dummy_idx_v = 1;
  size_t dummy_idx_vn = 1;
  size_t dummy_idx_vt = 1;
  readObjFile(ifs, dummy_idx_v, dummy_idx_vn, dummy_idx_vt, false);
}

Mesh::Mesh(const std::string &fname, std::ifstream& ifs, size_t &pre_idx_v, size_t &pre_idx_vn, size_t &pre_idx_vt)
    : m_vertices(), m_faces(), m_uv_coords(), m_normals(), fname(fname), nh_box(nullptr)
{
  readObjFile(ifs, pre_idx_v, pre_idx_vn, pre_idx_vt, true);
}

void Mesh::readObjFile(std::ifstream& ifs, size_t &pre_idx_v, size_t &pre_idx_vn, size_t &pre_idx_vt, bool check_o)
{
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;
  std::string s;
  std::string m_name;

  size_t sub_index_v = pre_idx_v;
  size_t sub_index_vn = pre_idx_vn;
  size_t sub_index_vt = pre_idx_vt;


  while (ifs >> code)
  {
    if (code == "v")
    {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back(glm::vec3(vx, vy, vz));
      
      min_vec.x = min(vx, (double)min_vec.x);
      min_vec.y = min(vy, (double)min_vec.y);
      min_vec.z = min(vz, (double)min_vec.z);

      max_vec.x = max(vx, (double)max_vec.x);
      max_vec.y = max(vy, (double)max_vec.y);
      max_vec.z = max(vz, (double)max_vec.z);
      
      pre_idx_v++;
    }
    else if (code == "vt")
    {
      ifs >> vx >> vy;
      // TODO:: ATTENTION FLIP VY
      m_uv_coords.push_back(glm::vec2(vx, 1-vy));
      pre_idx_vt++;

    }
    else if (code == "vn")
    {
      ifs >> vx >> vy >> vz;
      m_normals.push_back(glm::vec3(vx, vy, vz));
      pre_idx_vn++;
    }
    else if (code == "f")
    {
      size_t num_ind;
      size_t v_ind[3];
      size_t t_ind[3];
      size_t n_ind[3];
      
      for (int i=0; i<3; i++) {
        size_t indices[3]; // v/vt/vn
        ifs >> s;
        num_ind = splitF(s, indices);
        /*
          num_ind meaning:
          1 -> v only
          2 -> v && vt
          3 -> v && vt && vn
          4 -> v && vn
        */
        if (num_ind >= 1) v_ind[i] = indices[0]-sub_index_v;
  
        if (num_ind >= 2) {
          t_ind[i] = indices[1]-sub_index_vt;
          if (num_ind != 4) hasVt = true;
        } 

        if (num_ind >= 3) {
          n_ind[i] = indices[2]-sub_index_vn;
          hasVn = true;
        }
      }
      
      m_faces.push_back(MeshTriangle(m_name, v_ind, t_ind, n_ind));
    } 
    else if (code == "usemtl")
    {
      ifs >> s;
      mat_name = s;
      m_name = s;
    }
    else if (code == "g" || code == "o")
    {
      if (check_o) break;
    }
  }

  if (mat_name == "") std::cerr << "Empty Material" << std::endl;
  vec3 temp = max_vec - min_vec;

  nh_box = new NonhierBox(min_vec, temp);
}

Mesh::Mesh(const glm::vec3 &m_pos, vec3 &m_size)
    : m_vertices(), m_faces(), m_uv_coords(),
    fname("cube.obj"), nh_box(nullptr)
{
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::ifstream ifs((OBJ_DIR+fname).c_str());
  while (ifs >> code)
  {
    if (code == "v")
    {
      ifs >> vx >> vy >> vz;
      
      glm::vec3 vec = glm::vec3(vx * m_size.x, vy * m_size.y, vz * m_size.z);
      vec += m_pos;
      m_vertices.push_back(vec);
    }
    else if (code == "f")
    {
      ifs >> s1 >> s2 >> s3;
      m_faces.push_back(MeshTriangle(s1 - 1, s2 - 1, s3 - 1));
    }
  }
}

Mesh::~Mesh()
{
  if (nh_box)
    delete nh_box;
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

void Mesh::get_uv(HitRecord &rec)
{
   if (fname == "plane.obj")
  {
    rec.uv = vec2((rec.p.x + 1) / 2, (rec.p.z + 1) / 2);
    // std::cout << rec.u << " " << rec.v << std::endl;
  }
}

double Mesh::getArea()
{
  double area = 0.0;

  for (auto t : m_faces){
    vec3 v2v1 = m_vertices[t.v2] - m_vertices[t.v1];
    vec3 v3v1 = m_vertices[t.v3] - m_vertices[t.v1];

    area += 0.5 * length(cross(v2v1, v3v1)); 
  }
  return area;
}

glm::vec3 Mesh::getRandomPoint(int rand_idx)
{
  if (rand_idx < 0) rand_idx = random_interger(0, m_faces.size()-1);

  glm::vec3 p1 = m_vertices[m_faces[rand_idx].v1];
  glm::vec3 p2 = m_vertices[m_faces[rand_idx].v2];
  glm::vec3 p3 = m_vertices[m_faces[rand_idx].v3];

  glm::vec2 rand_coeff = random_for_light();
  return p1 + rand_coeff.x * (p2 - p1) + rand_coeff.y * (p3 - p1);

}


void Mesh::getRandomPointAndDirection(glm::mat4 &trans, glm::mat4 &t_invtrans, glm::vec3 &pt, glm::vec3 &direction)
{
  int rand_idx = random_interger(0, m_faces.size());
  if (rand_idx == m_faces.size()) rand_idx--;

  vec4 transPoint = trans * vec4(getRandomPoint(rand_idx), 1.0f);
  pt = vec3(transPoint.x, transPoint.y, transPoint.z);

  vec3 dir = random_direction_hemisphere();
  
  glm::vec3 p1 = m_vertices[m_faces[rand_idx].v1];
  glm::vec3 p2 = m_vertices[m_faces[rand_idx].v2];
  glm::vec3 p3 = m_vertices[m_faces[rand_idx].v3];
  vec4 transNormal = t_invtrans * vec4(cross(p2-p1, p3-p1), 0.0f);
  vec3 normal = normalize(vec3(transNormal.x, transNormal.y, transNormal.z));

  direction = orthonormalBasis(normal) * dir;
}


bool Mesh::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec)
{
  bool isIntersected = false;
  float tClosest = tmax;
  vec2 uv;
  float u, v; // Barycentric Coordinates
  std::string m_name;

  vec3 normal;
  vec3 shading_normal = vec3(0.0f);

#ifdef RENDER_BOUNDING_VOLUMES
  if (nh_box && !nh_box->intersected(ray, tmin, tmax, rec))
  {
    return false;
  } else if (nh_box) {
    return true;
  }
#else
  HitRecord temp;
  

  if (nh_box && !nh_box->intersected(ray, tmin, tmax, temp))
  {
    return isIntersected;
  }
#endif

  for (MeshTriangle triangle : m_faces)
  {
    float t;
    //  std::cout << triangle.v1 << ", " << triangle.v1 << ", " << triangle.v1<< std::endl;
    // std::cout << to_string( m_vertices[triangle.v1]) << ", " << to_string( m_vertices[triangle.v1]) << ", " << to_string( m_vertices[triangle.v1])<< std::endl;
    if (rayTriangleIntersect(ray.getOrigin(), ray.getDirection(),
                             m_vertices[triangle.v1], m_vertices[triangle.v2], m_vertices[triangle.v3], t, u, v) &&
        t < tClosest && t > tmin)
    {
      tClosest = t;
      isIntersected = true;
      
      normal = cross(m_vertices[triangle.v2] - m_vertices[triangle.v1], m_vertices[triangle.v3] - m_vertices[triangle.v1]);
      m_name = triangle.mat_name;
      if (!m_normals.empty()) shading_normal = m_normals[triangle.v1];
      // if (hasVt && useBary ) 
      // {
      //   uv = m_uv_coords[triangle.uv1] * u  + m_uv_coords[triangle.uv2] * v
      //     + (1-u-v) * m_uv_coords[triangle.uv3];
      //     //  std::cout << to_string(m_uv_coords[triangle.uv1]) << ", " <<  to_string(uv) << std::endl;
      // } else 
      if (hasVt)
      {
        uv = m_uv_coords[triangle.uv1];
      }
      if (hasVn && useBary)
      {
        // shading_normal = m_normals[triangle.n1] * u +  m_normals[triangle.n2] * v
        //   + (1-u-v) *m_normals[triangle.n3];
        shading_normal = m_normals[triangle.n1] ;
      }

    }
  
  }


  if (isIntersected)
  {
    // std::cout << fname << std::endl;
    rec.t = tClosest;
    rec.p = ray.at(rec.t);
    rec.set_face_normal(ray.getDirection(), normal);
    rec.shading_normal = shading_normal;

    if (m_name != "") mat_name = m_name;

    if (hasVt)
    {
      rec.uv = uv;
      // std::cout << "uv: " << to_string(uv) << std::endl;
    } else {
      get_uv(rec);
    }
  }

  return isIntersected;
}