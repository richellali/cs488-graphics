// Termm--Fall 2023

#include "PhongMaterial.hpp"
#include "utils.hpp"
#include <glm/ext.hpp>
#include <iostream>

#include "cs488-framework/MathUtils.hpp"

using namespace glm;

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess)
	: m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

bool PhongMaterial::scatter(Ray &ray, HitRecord &rec, Ray &reflected_ray, double &attenuation) {
	return false;
}

glm::vec3 PhongMaterial::kd()
{
	return m_kd;
}

glm::vec3 PhongMaterial::ks()
{
	return m_ks;
}

double PhongMaterial::p()
{
	return m_shininess;
}

ReflectiveMaterial::ReflectiveMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double reflective_radius)
	: reflective_radius(reflective_radius), PhongMaterial(kd, ks, shininess) {}




bool ReflectiveMaterial::scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation)
{
	if (reflective_radius == 0) { // purely diffuse refelction
		return false;
	}

	attenuation = this->attenuation;
	vec3 in_ray = normalize(ray.getDirection());
	vec3 normal = normalize(rec.normal);
	vec3 reflected_direction = in_ray - 2 * dot(in_ray, normal) * normal;

	if (reflective_radius == 1) { // purely specular
		out_ray = Ray(rec.p, reflected_direction);		
	} else {
		// Glossy
		vec3 fuzzy_sphere_point = radomPointOnFuzzySphere(rec.p, reflected_direction);
		out_ray = Ray(rec.p, fuzzy_sphere_point-rec.p);
		// std::cout << to_string(fuzzy_sphere_point-rec.p) << " " << to_string(reflected_direction) << std::endl;
		// std::cout << radiansToDegrees(dot(normalize(fuzzy_sphere_point-rec.p), normalize(reflected_direction))) << std::endl;
	}
	return true;
}

bool ReflectiveMaterial::isGlossy() {
	return 0 < reflective_radius && reflective_radius < 1;
}

vec3 ReflectiveMaterial::radomPointOnFuzzySphere(vec3 &reflected_point, vec3 &reflected_direction) {
	float fuzzy_angle = acos(reflective_radius);
	// std::cout << "fuzzy_angle: " << radiansToDegrees(fuzzy_angle) << std::endl;
	float l = 1 / tan(fuzzy_angle);
	// std::cout << "l: " << l << std::endl;


	vec3 outAtL = l * normalize(reflected_direction);
	// std::cout << "reflected_point: " << to_string(reflected_point) << std::endl;
	// std::cout << "reflected_direction: " << to_string(reflected_direction) << std::endl;
	// std::cout <<  "outAtL: " << to_string(outAtL)<< std::endl;
	vec3 sphere_center = reflected_point + outAtL;
	// std::cout << "sphere_center: " << to_string(sphere_center) << std::endl;

	vec3 ran = random_on_sphere();
	// std::cout << "random: " << to_string(ran) << std::endl;
	return ran + sphere_center;
}

DielectricMaterial::DielectricMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double refractive_index)
	: refractive_index(refractive_index), PhongMaterial(kd, ks, shininess) {}


bool DielectricMaterial::scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation)
{
	attenuation = 0.9;

	double refraction_ratio = rec.front_face ? (1.0 / refractive_index) : refractive_index;
	vec3 in_ray = normalize(ray.getDirection());
	vec3 normal = normalize(rec.normal);

	// check if sin_theta > 1, if greater, cannot refract
	double cos_theta = fmin(dot(-in_ray, normal), 1.0);
	bool cannot_refract = refraction_ratio * sqrt(1.0 - cos_theta * cos_theta);

	// schlick Approx
	auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
	r0 = r0*r0;
	double approx = r0 + (1-r0) * pow(1-cos_theta, 5);

	vec3 out_direction;

	if (cannot_refract || approx > random_double()) {
		out_direction = in_ray - 2 * dot(in_ray, normal) * normal; // reflection
	} else {
		vec3 out_perp = refraction_ratio * (in_ray + cos_theta * normal);
		vec3 out_parallel = -sqrt(fabs(1.0 - length2(out_perp))) * normal;
		out_direction = out_perp + out_parallel;
	}

	out_ray = Ray(rec.p, out_direction);

	rec.front_face = !rec.front_face;
	return true;
}