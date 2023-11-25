#include <iostream>
#include <fstream>

#include "BlenderMesh.hpp"

using namespace glm;

extern std::string OBJ_DIR;

BlenderMesh::BlenderMesh(const std::string &fname)
    : mesh_map(), fname(fname)
{
    std::ifstream ifs((OBJ_DIR+fname).c_str());
    std::string code;
    size_t pre_idx_v = 1;
    size_t pre_idx_vn = 1;
    size_t pre_idx_vt = 1;

    while (ifs >> code)
    {
        if (code == "g") break;
    }

    while (ifs >> code) 
    {
        // std::cout << "mesh name: " << code << std::endl;
        mesh_map[code] = new Mesh(fname, ifs, pre_idx_v, pre_idx_vn, pre_idx_vt);
    }
}

BlenderMesh::~BlenderMesh()
{
    for (auto &m : mesh_map) {
        delete m.second;
    }
}

bool BlenderMesh::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    float t=tmax;
    bool isIntersected = false;
    // std::cout << "b_mesh intersect" << std::endl;

    for (auto &m : mesh_map)
    {
        // if (m_name == "glass_Mesh") continue;
        // std::cout << m_name << std::endl;
        HitRecord tempRec;
        if (m.second->intersected(ray, tmin, t, tempRec)) {
            t = tempRec.t;

            // rec.t = tempRec.t;
            // rec.p = tempRec.p;
            // rec.normal = tempRec.normal;
            // rec.uv = tempRec.uv;
            rec = tempRec;

            rec.mat_name = m.second->mat_name;

            isIntersected = true;
        }
    }
    return isIntersected;
}
