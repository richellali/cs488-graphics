#include "Ray.hpp"

using namespace glm;

Ray::Ray() {
    origin = vec3(0.0f);
    direction = vec3(0.0f);
}

Ray::Ray(const vec3 ori, const vec3 dir)
: origin(ori),
  direction(dir)
  {}

vec3 Ray::getOrigin() const {
    return origin;
}

vec3 Ray::getDirection() const {
    return direction;
}

vec3 Ray::at(float t) const {
    return origin + t * direction;
}