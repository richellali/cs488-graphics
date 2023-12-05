#pragma once

#include "Integrator.hpp"
#include "A5.hpp"

class PathTracer : public Integrator
{
    public:
    PathTracer()
    {};
    virtual ~PathTracer(){};
    
    glm::vec3 ray_trace( Ray &ray, SceneNode *root, const glm::vec3 &ambient,
			   const std::list<Light *> &lights, const glm::vec3 &eye, int depth, const Texture *scene_text) override;
};