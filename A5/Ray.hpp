#pragma once

#include <glm/glm.hpp>

class Ray {
    public:
    Ray(const glm::vec3 ori, const glm::vec3 dir);
    Ray(const glm::vec3 ori, const glm::vec3 dir, bool front_face);
    Ray();

    glm::vec3 getOrigin() const;
    glm::vec3 getDirection() const;

    glm::vec3 at(float t) const;
    // Refraction
    bool front_face;

    private:
    glm::vec3 origin;
    glm::vec3 direction;
};