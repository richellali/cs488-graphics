// Termm--Fall 2023

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"
#include "utils.hpp"

Light::Light(glm::vec3 &_colour, LightType _type)
    : colour(_colour), position(glm::vec3(0.0f)), type(_type)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

Light::Light(glm::vec3 &col, double _falloff[3], glm::vec3 &pos, LightType _type)
    : colour(col), position(pos), type(_type)
{
  falloff[0] = _falloff[0];
  falloff[1] = _falloff[1];
  falloff[2] = _falloff[2];
}

PointLight::PointLight()
    : Light(LightType::POINT)
{
}

PointLight::PointLight(glm::vec3 &col, double _falloff[3], glm::vec3 &pos)
    : Light(col, _falloff, pos, LightType::POINT)
{
}

glm::vec3 PointLight::getRandomPoint()
{
  return position + random_on_sphere(3);
}

void PointLight::getRandomPointAndDirection(glm::vec3 &pt, glm::vec3 &direction){
  pt = getRandomPoint();
  direction = random_on_sphere();
  if (dot(-pt, direction) < 0) direction = -direction;
}
double PointLight::getArea() {return 1.0;};
glm::vec3 PointLight::getPower() {return colour;};

using namespace glm;
// std::string LIGHT_DIR = "light/";
// AreaLight::AreaLight(const std::string &fname)
//   : colour(vec3(0.0))
//   , m_vertices()
//   , m_faces()
//   , type(LightType::AREA)
// {
//   std::string code;
//   double vx, vy, vz;
//   size_t s1, s2, s3;

//   std::ifstream ifs((LIGHT_DIR+fname).c_str());

//   while (ifs >> code)
//   {
//     if (code == "v")
//     {
//       ifs >> vx >> vy >> vz;
//       m_vertices.push_back(glm::vec3(vx, vy, vz));
//     }
//     else if (code == "f")
//     {
//       ifs >> s1 >> s2 >> s3;
//       // std::cout << s1 << " " << s2 << " " << s3 << std:endl;
//       m_faces.push_back(Triangle(s1-1, s2-1, s3-1));
//     }
//     else if (code == "colour")
//     {
//       ifs >> vx >> vy >> vz;
//       colour = vec3(vx, vy, vz);
//     }
//     else if (code == "falloff")
//     {
//       ifs >> vx >> vy >> vz;
//       falloff[0] = vx;
//       falloff[1] = vy;
//       falloff[2] = vz;
//     }
//     else if (code == "density")
//     {
//       ifs >> s1;
//       light_density = s1;
//     }
//   }
// }

AreaLight::AreaLight(Mesh *_mesh, glm::vec3 _colour)
    : Light(_colour, LightType::AREA), m_mesh(_mesh), trans(mat4(1.0f)), t_invtrans(mat4(1.0f))
{
}

double AreaLight::getArea()
{
  return m_mesh->getArea();
}

// float AreaLight::getAtten()
// {
//   float area = getArea();
//   return falloff[0] + falloff[1] * area +
//      falloff[2] * area * area;
// }

vec3 AreaLight::getPower()
{
  return colour * getArea();
}

// size_t AreaLight::lightDenseNum(){
//   return light_density * m_faces.size();
// }

glm::vec3 AreaLight::getRandomPoint()
{
  vec4 transPoint = trans * vec4(m_mesh->getRandomPoint(), 1.0f);
  return vec3(transPoint.x, transPoint.y, transPoint.z);
}

void AreaLight::getRandomPointAndDirection(glm::vec3 &pt, glm::vec3 &direction) // TODO:: where to put normal
{
  m_mesh->getRandomPointAndDirection(trans, t_invtrans, pt, direction);

  // std::cout << to_string(direction) << std::endl;

  // pt = vec3(0.0f, 1.0f, 430.0f);
  // float pdf;
  // direction = random_direction_hemisphere(pdf);
  // direction = orthonormalBasis(vec3(0, 0, -1)) * direction;
}

// TODO
// std::ostream& operator<<(std::ostream& out, const Light& l)
// {
// if (l.type == LightType::AREA){
//   const AreaLight *a_light = static_cast<const AreaLight *>(&l);

//   out << "L[ Colour: {" << glm::to_string(a_light->colour) << "}, ";
//   for (Triangle triangle : a_light->m_faces)
//   {
//     out << "{"
//     << to_string(a_light->m_vertices[triangle.v1]) << ","
//     << to_string(a_light->m_vertices[triangle.v2]) << ","
//     << to_string(a_light->m_vertices[triangle.v3]) << "}, ";
//   }

//   out << "Falloff: ";

// }
// else if (l.type == LightType::POINT)
// {
//   const PointLight *p_light = static_cast<const PointLight *>(&l);
//   out << "L[" << glm::to_string(p_light->colour)
//   	  << ", " << glm::to_string(p_light->position) << ", ";
// }
//   for (int i = 0; i < 3; i++) {
//     if (i > 0) out << ", ";
//     out << l.falloff[i];
//   }
//   out << "]";
//   return out;
// }
