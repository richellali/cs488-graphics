#pragma once

#include <glm/glm.hpp>
#include "Material.hpp"
#include "Texture.hpp"

class HitRecord {
    public:
    double t;
    glm::vec3 normal;
    glm::vec3 p;

    Material *material;

    double u;
    double v;
    Texture *texture;

    // Refraction
    bool front_face=false;
};