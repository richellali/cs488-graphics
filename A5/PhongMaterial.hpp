// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

#include "Material.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
#include "Texture.hpp"

enum BRDFType
{
  DIFFUSE,
  SPECULAR
};

class PhongMaterial : public Material {
public:
  PhongMaterial();
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double roughness_index, double refractive_index);
  bool scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation, bool force_specular=false);
  virtual ~PhongMaterial();
  
  glm::vec3 kd(double u=0.0, double v=0.0);
  glm::vec3 ks(double u=0.0, double v=0.0);
  glm::vec3 ke();
  glm::vec3 ka();
  double p();

  void set_kd(glm::vec3 kd);
  void set_ks(glm::vec3 ks);
  void set_ke(glm::vec3 ke);
  void set_ka(glm::vec3 ka);
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
  bool isDiffuse();
  bool isGlossy();
  bool isEmissive(); // TODO

  glm::vec3 getReflectanceProb();

  void getReflectedRay(glm::vec3 &hit_point, glm::vec3 &in_ray, Ray &out_ray, glm::vec3 &normal, bool force_specular=false);
  bool getRefractedRay(HitRecord &rec, glm::vec3 &in_ray, Ray &out_ray, glm::vec3 &normal, bool force_specular=false);
  glm::vec3 radomPointOnFuzzySphere(glm::vec3 &reflected_point, glm::vec3 &reflected_direction);
  void getDiffuseReflected(HitRecord &rec, Ray &out_ray);

  glm::vec3 BRDF(Ray &ray, HitRecord &rec, Ray &out_ray, float &pdf);
  glm::vec3 sample_diffuse(glm::vec3 &normal, float &pdf);
  glm::vec3 evaluateBRDF(glm::vec3 wi, glm::vec3 wo);

  double attenuation;
  double m_shininess;

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_ke;
  glm::vec3 m_ka;
  glm::vec3 trans_colour;

  Texture *map_kd;
  Texture *map_ks;

  bool isSameHemiSphere(glm::vec3 &w, glm::vec3 &v);
  glm::vec3 f_diffuse(glm::vec3 &wi, glm::vec3 &wo);
  glm::vec3 f_perfect_reflect();
  glm::vec3 f_perfect_transmission();

  void sample_ray(HitRecord &rec, Ray &ray, Ray &out_ray, bool &goTrans, float &pdf);
  
};