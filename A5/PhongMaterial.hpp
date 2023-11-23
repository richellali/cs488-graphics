// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
#include "Texture.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial();
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double roughness_index, double refractive_index);
  bool scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation);
  virtual ~PhongMaterial();
  
  glm::vec3 kd(double u=0.0, double v=0.0);
  glm::vec3 ks(double u=0.0, double v=0.0);
  double p();

  void set_kd(glm::vec3 kd);
  void set_ks(glm::vec3 ks);
  void set_trans_colour(glm::vec3 colour);

  void set_map_kd(const std::string &map_kd_name);
  void set_map_ks(const std::string &map_ks_name);

  void set_kd_texture(Texture *texture);
  void set_ks_texture(Texture *texture);

  double roughness_index; // roughness
  double refractive_index; // -1 -> not transparent, 1 -> thin layer, Ni

  bool isReflected();
  bool isTransparent();
  bool isHollowGlass();
  bool isGlossy();
  glm::vec3 getReflectedRay(glm::vec3 &hit_point, glm::vec3 &in_ray, glm::vec3 &normal);
  glm::vec3 radomPointOnFuzzySphere(glm::vec3 &reflected_point, glm::vec3 &reflected_direction);

  double attenuation;
  double m_shininess;

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 trans_colour;

  Texture *map_kd;
  Texture *map_ks;
};