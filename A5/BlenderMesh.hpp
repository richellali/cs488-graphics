// Termm--Fall 2023

#pragma once

#include <map>
#include <string>
#include "HitRecord.hpp"

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "Mesh.hpp"


class BlenderMesh : public Primitive {
public:
  BlenderMesh(const std::string& fname );

  virtual ~BlenderMesh();

  bool intersected(Ray &ray, float tmin, float tmax, HitRecord &rec) override;
//   void get_uv(HitRecord &rec);
  
private:
    std::map<std::string, Mesh *> mesh_map;
    std::string fname;
};
