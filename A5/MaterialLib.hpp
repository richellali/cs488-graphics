#pragma once

#include "PhongMaterial.hpp"
#include <string.h>
#include <map>

class MaterialLib : public Material {
    public:
    MaterialLib(const std::string &fname);
    virtual ~MaterialLib();
    Material *get_mat(std::string &mat_name);

    private:
    std::map<std::string, PhongMaterial *> m_material_map;
};