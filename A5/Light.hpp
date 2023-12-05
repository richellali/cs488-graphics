// Termm--Fall 2023

#pragma once

#include <iosfwd>
#include <vector>
#include <string>
#include <fstream>
#include "Triangle.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
// #define RENDER_SOFT
enum LightType{
    NONE,
    POINT,
    AREA
  };

// Represents a simple point light.
class Light {
  public:
  
  Light()
  : colour(glm::vec3(0.0f)), type(LightType::NONE){}

  Light( LightType _type)
  : type(_type) {}

  Light(glm::vec3 &colour, LightType _type);
  Light(glm::vec3 &col, double _falloff[3], glm::vec3 &pos, LightType _type);

  virtual ~Light(){};

  virtual glm::vec3 getRandomPoint(){return position;}
  virtual void getRandomPointAndDirection(glm::vec3 &pt, glm::vec3 &direction)=0;
  virtual double getArea()=0;
  virtual glm::vec3 getPower()=0;

  LightType type;
  glm::vec3 position;
  glm::vec3 colour; // Ke emission in material
  double falloff[3];
};


class PointLight : public Light {
public:
  PointLight();
  PointLight(glm::vec3 &col, double _falloff[3], glm::vec3 &pos);

  virtual ~PointLight(){}

  glm::vec3 getRandomPoint() override;
  void getRandomPointAndDirection(glm::vec3 &pt, glm::vec3 &direction) override;
  double getArea() override;
  glm::vec3 getPower() override;
};

class AreaLight : public Light{
public:

  // AreaLight(const std::string &fname);
  AreaLight(Mesh *_mesh, glm::vec3 _colour);
  virtual ~AreaLight(){}

  glm::vec3 getRandomPoint() override;
  void getRandomPointAndDirection(glm::vec3 &pt, glm::vec3 &direction) override;
  double getArea() override;
  glm::vec3 getPower() override;

  glm::mat4 trans;
  glm::mat4 t_invtrans;

  private:
  Mesh * m_mesh;
};

std::ostream& operator<<(std::ostream& out, const Light& l);
