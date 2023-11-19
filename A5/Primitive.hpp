// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Ray.hpp"
#include "HitRecord.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec);
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
private:
 Primitive *nh_sphere;
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
private:
  Primitive *nh_box;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  
  virtual ~NonhierSphere();
  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
  void get_uv(HitRecord &rec);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  NonhierBox(const glm::vec3& pos, glm::vec3 &size);
  
  virtual ~NonhierBox();
  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
  void get_uv(HitRecord &rec);

private:
  glm::vec3 m_pos;
  glm::vec3 rec_size;
  Primitive *m_mesh;
  double m_size;
};

class NonhierCylinder : public Primitive {
  public: 
  NonhierCylinder(const glm::vec3& pos, double radius, double height)
    : m_pos(pos), m_radius(radius), m_height(height)
    {
    }

  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;

  private:
  glm::vec3 m_pos;
  double m_radius;
  double m_height;
};

class NonhierCone : public Primitive {
  public: 
  NonhierCone(const glm::vec3& pos, double radius, double height)
    : m_pos(pos), m_radius(radius), m_height(height)
    {
    }

  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;

  private:
  glm::vec3 m_pos;
  double m_radius;
  double m_height;
};