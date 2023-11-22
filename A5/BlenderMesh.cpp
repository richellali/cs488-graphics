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
    while (ifs >> code)
    {
        if (code == "g") break;
    }

    while (ifs >> code) 
    {
        mesh_map[code] = new Mesh(fname, ifs);
    }
}

BlenderMesh::~BlenderMesh()
{
}

bool BlenderMesh::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    return false;
}
