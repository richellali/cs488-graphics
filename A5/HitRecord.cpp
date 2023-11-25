#include "HitRecord.hpp"

using namespace glm;

void HitRecord::set_face_normal(const glm::vec3 &direction, glm::vec3 outward_normal)
{
    front_face = dot(direction, outward_normal) < 0;

    normal = front_face ? outward_normal : -outward_normal;
}