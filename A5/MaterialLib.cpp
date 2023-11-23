#include "MaterialLib.hpp"
#include <sstream>
#include <fstream>

using namespace glm;

std::string MTL_DIR = "mtl/";

MaterialLib::MaterialLib(const std::string &fname)
    : m_material_map()
{
    std::ifstream ifs((MTL_DIR+fname).c_str());

    std::string code, png_name, mtl_name;
    PhongMaterial *cur_mat;

    double vx, vy, vz;

    while (ifs >> code)
    {
        if (code == "newmtl")
        {
            ifs >> code;
            cur_mat = new PhongMaterial();
            m_material_map[code] = cur_mat;
        }
        else if (code == "Ks")
        {
            ifs >> vx >> vy >> vz;
            cur_mat->set_ks(vec3(vx, vy, vz));
        }
        else if (code == "Kd")
        {
            ifs >> vx >> vy >> vz;
            cur_mat->set_kd(vec3(vx, vy, vz));
        }
        else if (code == "map_Ks")
        {
            ifs >> code;
            cur_mat->set_map_ks(code);
        }
        else if (code == "map_Kd")
        {
            ifs >> code;
            cur_mat->set_map_kd(code);
        }
        else if (code == "Ni")
        {
            ifs >> vx;
            cur_mat->refractive_index = vx;
        }
        // else if (code == "d")
        // {
        //     ifs >> vx;
        //     if (vx == 0)
        //     {
        //         cur_mat->isTransparent = true;
        //     }
        // }
        else if (code == "Pr")
        {
            ifs >> vx;
            cur_mat->roughness_index = vx;
        }
        // else if (code == "Pm")
        // {
        //     // TODO
        // }
        else if (code == "Ns")
        {
            ifs >> vx;
            cur_mat->m_shininess = vx;
        }
    }
}

Material *MaterialLib::get_mat(std::string &mat_name)
{
    return m_material_map[mat_name];
}