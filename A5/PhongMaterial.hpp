// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"


class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual bool scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation);
  virtual ~PhongMaterial();
  

  glm::vec3 kd();
  glm::vec3 ks();
  double p();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double attenuation=0;
  double m_shininess;
};

class ReflectiveMaterial : public PhongMaterial {
  public:
  ReflectiveMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflective_radius);
  bool scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation) override;
  bool isGlossy();
  glm::vec3 radomPointOnFuzzySphere(glm::vec3 &reflected_point, glm::vec3 &reflected_direction);

  private:
  double attenuation=0.3;
  double reflective_radius;
};

class DielectricMaterial : public PhongMaterial {
  public:
  DielectricMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double refractive_index);
  bool scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation) override;

  private:
  double refractive_index;
};