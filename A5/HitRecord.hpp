#pragma once

#include <glm/glm.hpp>
#include "Material.hpp"
#include "Texture.hpp"
#include <string.h>

class HitRecord {
    public:
    double t=std::numeric_limits<float>::max();
    glm::vec3 normal;
    glm::vec3 shading_normal;
    glm::vec3 p;

    Material *material;
    std::string mat_name;

    glm::vec2 uv;
    Texture *texture; // TODO:: delete

    bool front_face;
    void set_face_normal(const glm::vec3 &direction, glm::vec3 normal);

};