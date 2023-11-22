#include "Ray.hpp"

using namespace glm;

Ray::Ray() {
    origin = vec3(0.0f);
    direction = vec3(0.0f);
    front_face = true;
}

Ray::Ray(const vec3 ori, const vec3 dir)
: origin(ori),
  direction(dir),
  front_face(true)
  {}

Ray::Ray(const vec3 ori, const vec3 dir, bool front_face)
: origin(ori),
  direction(dir),
  front_face(front_face)
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