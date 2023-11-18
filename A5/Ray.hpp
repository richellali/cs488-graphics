#pragma once

#include <glm/glm.hpp>

class Ray {
    public:
    Ray(const glm::vec3 ori, const glm::vec3 dir);
    Ray();

    glm::vec3 getOrigin() const;
    glm::vec3 getDirection() const;

    glm::vec3 at(float t) const;

    private:
    glm::vec3 origin;
    glm::vec3 direction;
};