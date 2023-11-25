#include "utils.hpp"
#include <climits>
#include <random>
#include <glm/ext.hpp>

using namespace glm;

/*
 * ANTIALIASING: https://raytracing.github.io/books/RayTracingInOneWeekend.html#antialiasing
 */
double random_double()
{
	return rand() / (RAND_MAX + 1.0);
}

double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

vec3 random_in_unit_disk()
{
	while (true)
	{
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (length2(p) < 1)
			return p;
	}
}

vec2 random_for_light()
{
	while (true)
	{
		auto p = vec2(random_double(0, 1), random_double(0, 1));
		if (p.x + p.y <= 1)
			return p;
	}
}

vec3 random_on_sphere()
{
	while (true)
	{
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
		if (length2(p) < 1)
			return p;
	}
}
