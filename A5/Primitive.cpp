// Termm--Fall 2023

#include "Primitive.hpp"
#include "Mesh.hpp"
#include "polyroots.hpp"
#include <iostream>
#include <glm/ext.hpp>
#include "utils.hpp"

extern const float Pi;
using namespace glm;

float min_but_greater_than_tmin(float tmin, float t1, float t2) {
    if (t1 > tmin && t2 > tmin) {
        return min(t1, t2);
    } else if (t1 > tmin) {
        return t1;
    } else {
        return t2;
    }
}

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

    rec.uv = vec2(0.5 + atan2(d.z, d.x) / (2*Pi), 0.5 + asin(d.y) / Pi);
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
        t = min_but_greater_than_tmin(tmin, roots[0], roots[1]);
    }

    // if (t < tmin || t > tmax) return false;

    rec.t = t;
    rec.p = ray.at(t);
    // rec.normal = rec.p - m_pos;
    rec.set_face_normal(ray.getDirection(), (rec.p-m_pos)/m_radius);
    get_uv(rec);
    if (t < tmin || t > tmax) return false;

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
            // rec.u = normP.z;
            // rec.v = normP.y;
            rec.uv = vec2(normP.z, normP.y);

        } else if ((normP.y <= 0.0001 || normP.y >= 0.9999)) {
            // rec.u = normP.x;
            // rec.v = normP.z;
            rec.uv = vec2(normP.x, normP.z);
        } else {
            // rec.u = normP.x;
            // rec.v = normP.y;
            rec.uv = vec2(normP.x, normP.y);
        }
        return true;
    } 
    return false;
}

bool NonhierCylinder::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    bool isIntersected = false;

    vec3 center_vec = ray.getOrigin() - m_pos;

    double A = ray.getDirection().x * ray.getDirection().x + ray.getDirection().y * ray.getDirection().y;
    double B = 2 * (ray.getDirection().x*center_vec.x + ray.getDirection().y*center_vec.y);
    double C = center_vec.x*center_vec.x + center_vec.y*center_vec.y - m_radius*m_radius;

    double roots[2];

    size_t numRoots = quadraticRoots(A, B, C, roots);

    double t = -1;

    double zmin = 0;
    double zmax = m_height;

    if (numRoots == 1) {
        double z = center_vec.z + roots[0] * ray.getDirection().z;
        if (zmin < z && z < zmax) {
            t = roots[0];
        }
    } else if (numRoots == 2) {
        double z1 = center_vec.z + roots[0] * ray.getDirection().z;
        double z2 = center_vec.z + roots[1] * ray.getDirection().z;

        if ((zmin  < z1 && z1 < zmax) && (zmin < z2 && z2 < zmax)) {
            t = min_but_greater_than_tmin(tmin, roots[0], roots[1]);
        } else if (zmin  < z1 && z1 < zmax) {
            t = roots[0];
        } else if (zmin < z2 && z2 < zmax) {
            t = roots[1];
        }
    }

    if (tmin <= t && t <= tmax) {
        rec.t = t;
        rec.p = ray.at(t);
        vec3 va = vec3(0, 0, 1);
        vec3 normal = (rec.p - m_pos) - dot(va, rec.p-m_pos) * va;

        rec.set_face_normal(ray.getDirection(), normal);

        isIntersected = true;
    }

    // min cap
    double cap_t = (zmin - center_vec.z) / ray.getDirection().z;
    double cap_x = center_vec.x + cap_t * ray.getDirection().x;
    double cap_y = center_vec.y + cap_t * ray.getDirection().y;

    if (cap_x * cap_x + cap_y * cap_y <= m_radius * m_radius && (tmin <= cap_t) && (cap_t <= tmax)) {
        if (!(isIntersected && cap_t > rec.t)) {
            rec.t = cap_t;
            rec.p = ray.at(cap_t);
            // rec.normal = vec3(0, 0, -1);

            rec.set_face_normal(ray.getDirection(), vec3(0, 0, -1));
            isIntersected = true;
        } 
        
    }

    // max cap
    cap_t = (zmax - center_vec.z) / ray.getDirection().z;
    cap_x = center_vec.x + cap_t * ray.getDirection().x;
    cap_y = center_vec.y + cap_t * ray.getDirection().y;

    if (cap_x * cap_x + cap_y * cap_y <= m_radius * m_radius && (tmin <= cap_t) && (cap_t <= tmax)) {
        if (!(isIntersected && cap_t > rec.t)) {
            rec.t = cap_t;
            rec.p = ray.at(cap_t);
            // rec.normal = vec3(0, 0, 1);
            rec.set_face_normal(ray.getDirection(), vec3(0, 0, 1));
            isIntersected = true;
        } 
    }


    return isIntersected;
}

bool NonhierCone::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    bool isIntersected = false;

    vec3 center_vec = ray.getOrigin() - m_pos;

    double r_sq = m_radius * m_radius;

    double A = ray.getDirection().x * ray.getDirection().x + ray.getDirection().y * ray.getDirection().y
            - ray.getDirection().z * ray.getDirection().z / r_sq;
    double B = 2 * (ray.getDirection().x*center_vec.x + ray.getDirection().y*center_vec.y
            - ray.getDirection().z*center_vec.z / r_sq);
    double C = center_vec.x*center_vec.x + center_vec.y*center_vec.y - center_vec.z*center_vec.z/r_sq; 

    double roots[2];

    size_t numRoots = quadraticRoots(A, B, C, roots);

    double t = -1;

    double zmin = 0;
    double zmax = m_height;

     if (numRoots == 1) {
        double z = center_vec.z + roots[0] * ray.getDirection().z;
        if (zmin < z && z < zmax) {
            t = roots[0];
        }
    } else if (numRoots == 2) {
        double z1 = center_vec.z + roots[0] * ray.getDirection().z;
        double z2 = center_vec.z + roots[1] * ray.getDirection().z;

        if ((zmin  < z1 && z1 < zmax) && (zmin < z2 && z2 < zmax)) {
            t = min_but_greater_than_tmin(tmin, roots[0], roots[1]);
        } else if (zmin  < z1 && z1 < zmax) {
            t = roots[0];
        } else if (zmin < z2 && z2 < zmax) {
            t = roots[1];
        }
    }

    if (tmin <= t && t <= tmax) {
        rec.t = t;
        rec.p = ray.at(t);

        vec3 va = vec3(0, 0, 1);
        vec3 m = (rec.p - m_pos) - dot(va, rec.p-m_pos) * va;
        double alpha = atan2(m_radius, m_height);
        vec3 normal = cos(alpha)*m + sin(alpha)*va;
        rec.set_face_normal(ray.getDirection(), normal);

        isIntersected = true;
    }

    // upper cap
    double cap_t = (zmax - center_vec.z) / ray.getDirection().z;
    double cap_x = center_vec.x + cap_t * ray.getDirection().x;
    double cap_y = center_vec.y + cap_t * ray.getDirection().y;

    if (cap_x * cap_x + cap_y * cap_y <= zmax*zmax/r_sq && (tmin <= cap_t) && (cap_t <= tmax)) {
        if (!(isIntersected && cap_t > rec.t)) {
            rec.t = cap_t;
            rec.p = ray.at(cap_t);
            // rec.normal = vec3(0, 0, 1);
            rec.set_face_normal(ray.getDirection(), vec3(0, 0, 1));
            isIntersected = true;
        } 
        
    }

    return isIntersected;
}