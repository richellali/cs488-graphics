#include "utils.hpp"
#include <climits>
#include <iostream>
#include <random>
#include <glm/ext.hpp>

using namespace glm;


static thread_local std::mt19937 *GENERATOR = nullptr;
static std::uniform_real_distribution<double> r_distribution(0, 1.0);

void initRNG(){
	if (GENERATOR) delRNG();
	std::random_device rd;
	GENERATOR = new std::mt19937(rd());
}

void delRNG(){
	if (GENERATOR != nullptr) delete GENERATOR;
	GENERATOR  = nullptr;
}

double random_double()
{
	if (GENERATOR == nullptr) {
		std::cerr <<  "ERROR! Thread generator not allocated." << std::endl;
		std::random_device rd;
		GENERATOR = new std::mt19937(rd());
	}

	return r_distribution(*GENERATOR);
}

double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

int random_interger(int min, int max)
{
	std::uniform_int_distribution<int> i_distribution(min, max);
	return i_distribution(*GENERATOR);
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
		auto p = vec2(random_double(), random_double());
		if (p.x + p.y <= 1)
			return p;
	}
}

vec3 random_on_sphere(double r)
{
	while (true)
	{
		auto p = vec3(random_double(-r, r), random_double(-r, r), random_double(-r, r));
		if (length2(p) < r*r)
			return p;
	}
}

vec3 random_direction_hemisphere()
{
	double r1 = random_double();
	double r2 = random_double();

	double phi = 2 * Pi * r1;
	double theta = acos(sqrt(r2));

	return vec3(
		cos(phi) * sqrt(1-r2),
		sin(phi) * sqrt(1-r2),
		sqrt(r2)
	);
}

// Building an Orthonormal Basis, Revisited. - Duff et al.
// https://graphics.pixar.com/library/OrthonormalB/paper.pdf
glm::dmat3 orthonormalBasis(const glm::dvec3& n)
{
    double sign = std::copysign(1.0, n.z);
    double a = -1.0 / (sign + n.z);
    double b = n.x * n.y * a;
    return
    {
        { 1.0 + sign * n.x * n.x * a, sign * b, -sign * n.x },
        { b, sign + n.y * n.y * a, -n.y },
        n
    };
}

vec3 comp_mul(vec3 &v1, vec3 &v2)
{
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

void russian_roulette_prob(vec3 &kd, vec3 &ks, float &Pd, float &Ps, float &Pa)
{
	float Pr = max(kd.r + ks.r, kd.g + ks.g, kd.b + ks.b);
    Pd = compAdd(kd) / (compAdd(kd) + compAdd(ks)) * Pr; // prob of diffuse
    Ps = Pr - Pd;                                        // prob of specular
    Pa = 0.005;
}