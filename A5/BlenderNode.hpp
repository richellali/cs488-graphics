// Termm--Fall 2023

#pragma once

#include <map>
#include <string.h>

#include "SceneNode.hpp"
#include "MaterialLib.hpp"
#include "Primitive.hpp"


class BlenderNode : public SceneNode {
public:
    BlenderNode( const std::string & name, Primitive *prim);

    void setMaterialLib( MaterialLib *mat_lib);

    virtual ~BlenderNode();
    
    Primitive *m_primitive;
    MaterialLib *m_mat_lib;


    bool intersected(Ray &ray, float tmin, HitRecord &rec) override;
};