#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

constexpr float Pi = 3.14159265358979323846;
constexpr float invPi = 0.31830988618379067154;

void initRNG();
void delRNG();

double random_double();

double random_double(double min, double max);

glm::vec3 random_in_unit_disk();

glm::vec2 random_for_light();

glm::vec3 random_on_sphere(double r=1);

glm::vec3 random_direction_hemisphere();

int random_interger(int min, int max);

glm::dmat3 orthonormalBasis(const glm::dvec3& n);

glm::vec3 comp_mul(glm::vec3 &v1, glm::vec3 &v2);

void russian_roulette_prob(glm::vec3 &kd, glm::vec3 &ks, float &Pd, float &Ps, float &Pa);