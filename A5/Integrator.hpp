#pragma once

#include <glm/glm.hpp>

#include "Ray.hpp"
#include "SceneNode.hpp"
#include "Light.hpp"
#include "Texture.hpp"

class Integrator {
public:
    Integrator(){};
    virtual ~Integrator(){};

    virtual bool produceImage(Ray &ray){return false;}

    virtual glm::vec3 ray_trace(
        Ray &ray, SceneNode *root, const glm::vec3 &ambient,
			   const std::list<Light *> &lights, const glm::vec3 &eye, int depth, const Texture *scene_text)
               {
                return glm::vec3(0.0f);
               }

};