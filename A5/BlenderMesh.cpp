#include <iostream>
#include <fstream>

#include "BlenderMesh.hpp"

using namespace glm;

extern std::string OBJ_DIR;

BlenderMesh::BlenderMesh(const std::string &fname)
    : mesh_map(), light_map(), fname(fname)
{
    std::ifstream ifs((OBJ_DIR+fname).c_str());
    // std::cout << fname << std::endl;
    std::string code;
    size_t pre_idx_v = 1;
    size_t pre_idx_vn = 1;
    size_t pre_idx_vt = 1;

    while (ifs >> code)
    {
        if (code == "g" || code == "o") break;
    }

    while (ifs >> code) 
    {
        std::cout << "mesh name: " << code << std::endl;
        mesh_map[code] = new Mesh(fname, ifs, pre_idx_v, pre_idx_vn, pre_idx_vt);
        if (code == "table_Mesh" ) mesh_map[code]->useBary=false;
    }
}

BlenderMesh::~BlenderMesh()
{
    for (auto &m : mesh_map) {
        delete m.second;
    }

    for (auto &l : light_map)
    {
        delete l.second;
    }
}

bool BlenderMesh::intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) {
    float t=tmax;
    bool isIntersected = false;
    // std::cout << "b_mesh intersect" << std::endl;

    for (auto &m : mesh_map)
    {
        
        if (!(  m.first == "tab1_Mesh" 
        || m.first == "Podest_Mesh"
        // || m.first == "snowman1_Mesh"
        )) continue;

        

        // if (m.second->isLight) continue;
        // std::cout << m_name << std::endl;
        HitRecord tempRec;
        if (m.second->intersected(ray, tmin, t, tempRec)) {
            t = tempRec.t;
            rec = tempRec;

            rec.mat_name = m.second->mat_name;

            isIntersected = true;
        }
    }
    return isIntersected;
}

void BlenderMesh::collect_lights(std::list<AreaLight *> &lights, 
    std::map<std::string, glm::vec3> &emissiveNamesAndProps) // mat_name, mat_ke
{
    // std::map<std::string, Mesh *> mesh_map;
    for (auto m : mesh_map)
    {
        auto it = emissiveNamesAndProps.find(m.second->mat_name);
        if (it != emissiveNamesAndProps.end())
        {
            m.second->isLight = true;
            light_map[m.first] = new AreaLight(m.second, it->second);
            lights.push_back(light_map[m.first]);
        }
    }
}
