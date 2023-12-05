// Termm--Fall 2023

#pragma once

#include <map>
#include <string.h>

#include "SceneNode.hpp"
#include "MaterialLib.hpp"
#include "BlenderMesh.hpp"


class BlenderNode : public SceneNode {
public:
    BlenderNode( const std::string & name, BlenderMesh *prim);

    void setMaterialLib( MaterialLib *mat_lib);

    virtual ~BlenderNode();
    
    BlenderMesh *m_b_mesh;
    MaterialLib *m_mat_lib;

    bool intersected(Ray &ray, float tmin, HitRecord &rec) override;
    void collect_lights(std::list<AreaLight *> &lights) override;
};