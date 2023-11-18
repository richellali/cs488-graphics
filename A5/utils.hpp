#pragma once

#include <glm/glm.hpp>

/*
 * ANTIALIASING: https://raytracing.github.io/books/RayTracingInOneWeekend.html#antialiasing
 */
double random_double();

double random_double(double min, double max);

glm::vec3 random_in_unit_disk();

glm::vec3 random_on_sphere();
