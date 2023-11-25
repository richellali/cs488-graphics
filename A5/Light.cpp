// Termm--Fall 2023

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

using namespace glm;

std::string LIGHT_DIR = "light/";

#ifdef RENDER_SOFT_SHADOW
Light::Light(const std::string &fname)
  : colour(vec3(0.0))
  , m_vertices()
  , m_faces()
{
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::ifstream ifs((LIGHT_DIR+fname).c_str());

  while (ifs >> code)
  {
    if (code == "v")
    {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back(glm::vec3(vx, vy, vz));
    }
    else if (code == "f")
    {
      ifs >> s1 >> s2 >> s3;
      // std::cout << s1 << " " << s2 << " " << s3 << std:endl;
      m_faces.push_back(LightTriangle(s1-1, s2-1, s3-1));
    }
    else if (code == "colour")
    {
      ifs >> vx >> vy >> vz;
      colour = vec3(vx, vy, vz);
    }
    else if (code == "falloff")
    {
      ifs >> vx >> vy >> vz;
      falloff[0] = vx;
      falloff[1] = vy;
      falloff[2] = vz;
    }
    else if (code == "density")
    {
      ifs >> s1;
      light_density = s1;
    }
  }
}

size_t Light::lightDenseNum(){
  return light_density * m_faces.size();
}

#else
Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}
#endif

std::ostream& operator<<(std::ostream& out, const Light& l)
{
#ifdef RENDER_SOFT_SHADOW
  out << "L[ Colour: {" << glm::to_string(l.colour) << "}, ";
  for (LightTriangle triangle : l.m_faces)
  {
    out << "{" 
    << to_string(l.m_vertices[triangle.v1]) << ","
    << to_string(l.m_vertices[triangle.v2]) << ","
    << to_string(l.m_vertices[triangle.v3]) << "}, ";
  }


  out << "Falloff: ";

#else
  out << "L[" << glm::to_string(l.colour) 
  	  << ", " << glm::to_string(l.position) << ", ";
#endif
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
