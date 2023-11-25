// Termm--Fall 2023

#pragma once

#include <iosfwd>
#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
// #define RENDER_SOFT_SHADOW

struct LightTriangle
{
  size_t v1;
	size_t v2;
	size_t v3;

  LightTriangle( size_t pv1, size_t pv2, size_t pv3 )
  : v1(pv1)
  , v2(pv2)
  , v3(pv3)
  {}
};

// Represents a simple point light.
class Light {
public:
#ifdef RENDER_SOFT_SHADOW
  Light(const std::string &fname);
  std::vector<glm::vec3> m_vertices;
	std::vector<LightTriangle> m_faces;

  size_t lightDenseNum();

  size_t light_density;
#else
  Light();
  glm::vec3 position;
#endif 

  glm::vec3 colour; 
  double falloff[3];
};

std::ostream& operator<<(std::ostream& out, const Light& l);
