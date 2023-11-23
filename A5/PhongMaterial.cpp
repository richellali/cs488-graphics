// Termm--Fall 2023

#include "PhongMaterial.hpp"
#include "utils.hpp"
#include <glm/ext.hpp>
#include <iostream>

#include "cs488-framework/MathUtils.hpp"

using namespace glm;

PhongMaterial::PhongMaterial()
	: m_kd(vec3(0.0f))
	, m_ks(vec3(0.0f))
	, trans_colour(vec3(0.0f))
	, map_kd(nullptr)
	, map_ks(nullptr)
	, roughness_index(0.0)
	, refractive_index(-1)
	, attenuation(0)
	, m_shininess(0)
{}

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, 
	double shininess, double roughness_index, double refractive_index)
	: m_kd(kd)
	, m_ks(ks)
	, trans_colour(vec3(0.0f))
	, map_kd(nullptr)
	, map_ks(nullptr)
	, roughness_index(roughness_index)
	, refractive_index(refractive_index)
	, attenuation(0)
	, m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial() {
	if (map_kd != nullptr) delete map_kd;
	if (map_ks != nullptr) delete map_ks;
}

// -----------------------------------------------------------
glm::vec3 PhongMaterial::kd(double u, double v) { 
	if (map_kd == nullptr) return m_kd;
	// std::cout << "Texture: " << u << ", " << v << std::endl;
	return map_kd->colour(u, v); 
}

glm::vec3 PhongMaterial::ks(double u, double v) { 
	if (map_ks == nullptr) return m_ks;
	return map_ks->colour(u, v); 
}

double PhongMaterial::p() { return m_shininess; }

void PhongMaterial::set_kd(glm::vec3 kd) {
	m_kd = kd;
}

void PhongMaterial::set_ks(glm::vec3 ks) {
	m_ks = ks;
}

void PhongMaterial::set_trans_colour(glm::vec3 colour) {
	trans_colour = colour;
}

void PhongMaterial::set_map_kd(const std::string &map_kd_name) {
	if (map_kd_name == "") return;
	map_kd = new Texture(map_kd_name);

}

void PhongMaterial::set_map_ks(const std::string &map_ks_name) {
	if (map_ks_name == "") return;
	map_ks = new Texture(map_ks_name);
}

void PhongMaterial::set_kd_texture(Texture *texture) {
	map_kd = texture;
}

void PhongMaterial::set_ks_texture(Texture *texture) {
	map_ks = texture;
}

// -----------------------------------------------------------
bool PhongMaterial::isReflected() {
	return roughness_index > 0.9;
}

bool PhongMaterial::isTransparent() {
	return refractive_index != -1;
}

bool PhongMaterial::isHollowGlass() {
	return refractive_index == 1;
}

bool PhongMaterial::isGlossy() {
	return 0 < roughness_index && roughness_index < 1;
}


// --------------------------------------------------------------
vec3 PhongMaterial::getReflectedRay(vec3 &hit_point, vec3 &in_ray, vec3 &normal)
{
	vec3 reflected_direction = in_ray - 2 * dot(in_ray, normal) * normal;

	if (isGlossy()) {
		// Glossy
		vec3 fuzzy_sphere_point = radomPointOnFuzzySphere(hit_point, reflected_direction);
		reflected_direction = fuzzy_sphere_point-hit_point;
		// std::cout << to_string(fuzzy_sphere_point-hit_point) << " " << to_string(reflected_direction) << std::endl;
		// std::cout << radiansToDegrees(dot(normalize(fuzzy_sphere_point-hit_point), normalize(reflected_direction))) << std::endl;
	}
	return reflected_direction;
}

vec3 PhongMaterial::radomPointOnFuzzySphere(vec3 &reflected_point, vec3 &reflected_direction) {
	float fuzzy_angle = acos(roughness_index);
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

bool PhongMaterial::scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation) {
	// if ri == 1 & transparent -> let it leaves

	if (!(isReflected() || isTransparent())) {
		return false;
	}

	vec3 normal = ray.front_face ? normalize(rec.normal) : -normalize(rec.normal);

	if (isHollowGlass()) { // Might fix using other methods later
		attenuation = 0.9;
		out_ray = Ray(rec.p - normal * 0.00001f, ray.getDirection(), ray.front_face);
		return true;
	}
	
	vec3 in_ray = normalize(ray.getDirection());

	// // schlick Approx
	// auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
	// r0 = r0*r0;
	// float approx_theta = fmin(dot(-in_ray, normalize(rec.normal)), 1.0);
	// double approx = r0 + (1-r0) * pow(1-approx_theta, 5);

	if (isTransparent()) 
	{
		float refraction_ratio = ray.front_face ? 1.0 / refractive_index : refractive_index;
		double cos_theta = dot(in_ray, normal);
		float k = 1.0f - refraction_ratio * refraction_ratio * (1.0f - cos_theta * cos_theta);

		if (k <= 0) {
			// attenuation = 0.2;
			// vec3 out_direction = getReflectedRay(in_ray, normal); // reflection
	 		// out_ray = Ray(rec.p , out_direction, ray.front_face);
			return false;
		} else {
			attenuation = 0.9;
			vec3 out_direction = refraction_ratio * in_ray - (refraction_ratio * cos_theta + sqrt(k)) * normal;
			out_ray = Ray(rec.p - normal * 0.001f, out_direction, !ray.front_face);
		}
	} else if (isReflected())
	{
		attenuation = 0.2;
		vec3 out_direction = getReflectedRay(rec.p, in_ray, normal);
		out_ray = Ray(rec.p, out_direction, ray.front_face);
	}

	return true;
}