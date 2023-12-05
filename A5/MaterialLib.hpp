#pragma once

#include "PhongMaterial.hpp"
#include <string.h>
#include <map>
#include <vector>

class MaterialLib : public Material {
    public:
    MaterialLib(const std::string &fname);
    virtual ~MaterialLib();
    Material *get_mat(std::string &mat_name);

    std::map<std::string, glm::vec3> getEmissiveNamesAndKe();

    private:
    std::map<std::string, PhongMaterial *> m_material_map;
};