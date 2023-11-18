// Termm--Fall 2023

#include "Primitive.hpp"
#include "Mesh.hpp"
#include "polyroots.hpp"
#include <iostream>

using namespace glm;

Primitive::~Primitive()
{
}

bool Primitive::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    return false;
}

Sphere::Sphere() {
    nh_sphere = new NonhierSphere(vec3(0.0), 1.0);
}

Sphere::~Sphere()
{
    if (nh_sphere) delete nh_sphere;
}

bool Sphere::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    return nh_sphere->intersected(ray, tmin, tmax, rec);
}

Cube::Cube() {
    nh_box = new NonhierBox(vec3(0.0), 1.0);
}

Cube::~Cube()
{
    if (nh_box) delete nh_box;
}

bool Cube::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    return nh_box->intersected(ray, tmin, tmax, rec);
}

NonhierSphere::~NonhierSphere()
{
}

void NonhierSphere::get_uv(HitRecord &rec) {
    vec3 d = normalize(m_pos - rec.p);
    double PI = 3.1415926;

    rec.u = 0.5 + atan2(d.z, d.x) / (2*PI);
    rec.v = 0.5 + asin(d.y) / PI;

}

bool NonhierSphere::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {

    vec3 center_vec = ray.getOrigin() - m_pos;

    double A = dot(ray.getDirection(), ray.getDirection());
    double B = 2 * dot(ray.getDirection(), center_vec);
    double C = dot(center_vec, center_vec) - m_radius * m_radius;

    double roots[2];

    size_t numRoots = quadraticRoots(A, B, C, roots);

    double t;

    if (numRoots == 0 ) {
        return false;
    } else if (numRoots == 1) {
        t = roots[0];
    } else {
        t = min(roots[0], roots[1]);
    }

    if (t < tmin || t > tmax) return false;

    rec.t = t;
    rec.p = ray.at(t);
    rec.normal = rec.p - m_pos;
    get_uv(rec);

    return true;

}

NonhierBox::NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size), rec_size(vec3(size, size, size))
  {
    m_mesh = new Mesh(pos, rec_size);
  }

  NonhierBox::NonhierBox(const glm::vec3& pos, glm::vec3 &size)
    : m_pos(pos), m_size(1.0), rec_size(size)
  {
    m_mesh = new Mesh(pos, size);
  }

NonhierBox::~NonhierBox()
{
    if (m_mesh) delete m_mesh;
}

bool NonhierBox::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    if (m_mesh->intersected(ray, tmin, tmax, rec)){
        vec3 normP = (rec.p - m_pos) / (float)m_size;

        if (normP.x <= 0.0001 || normP.x >= 0.9999) {
            rec.u = normP.z;
            rec.v = normP.y;

        } else if ((normP.y <= 0.0001 || normP.y >= 0.9999)) {
            rec.u = normP.x;
            rec.v = normP.z;
        } else {
            rec.u = normP.x;
            rec.v = normP.y;
        }
        return true;
    } 
    return false;
}