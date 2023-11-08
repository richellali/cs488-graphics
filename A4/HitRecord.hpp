#pragma once

#include <glm/glm.hpp>
#include "Material.hpp"

class HitRecord {
    public:
    glm::vec3 normal;
    double t;
    glm::vec3 p;
    Material *material;
};