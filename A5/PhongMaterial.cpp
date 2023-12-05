// Termm--Fall 2023

#include "PhongMaterial.hpp"
#include "utils.hpp"
#include <glm/ext.hpp>
#include <iostream>

#include "cs488-framework/MathUtils.hpp"

extern const float Pi;
extern const float invPi;
using namespace glm;

PhongMaterial::PhongMaterial()
	: m_kd(vec3(0.0f))
	, m_ks(vec3(0.0f))
	, m_ke(vec3(0.0f))
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
	, m_ke(vec3(0.0f))
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

glm::vec3 PhongMaterial::ke() { 
	return m_ke; 
}

glm::vec3 PhongMaterial::ka() { 
	return m_ka; 
}

double PhongMaterial::p() { return m_shininess; }

void PhongMaterial::set_kd(glm::vec3 kd) {
	m_kd = kd;
}

void PhongMaterial::set_ks(glm::vec3 ks) {
	m_ks = ks;
}

void PhongMaterial::set_ke(glm::vec3 ke) {
	m_ke = ke;
}

void PhongMaterial::set_ka(glm::vec3 ka) {
	m_ka = ka;
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
bool PhongMaterial::isDiffuse() {
	return !(isReflected() || isTransparent());
}

bool PhongMaterial::isReflected() {
	return roughness_index > 0.9;
}

bool PhongMaterial::isTransparent() {
	return refractive_index > 0;
}

bool PhongMaterial::isHollowGlass() {
	return refractive_index == 1;
}

bool PhongMaterial::isGlossy() {
	return 0 < roughness_index && roughness_index < 1;
}

bool PhongMaterial::isEmissive() {
	return m_ke != vec3(0.0f);
}

// --------------------------------------------------------------
vec3 PhongMaterial::radomPointOnFuzzySphere(vec3 &reflected_point, vec3 &reflected_direction) {
	float fuzzy_angle = acos(roughness_index);
	float l = 1 / tan(fuzzy_angle);
	vec3 outAtL = l * normalize(reflected_direction);

	vec3 sphere_center = reflected_point + outAtL;
	vec3 ran = random_on_sphere();
	return ran + sphere_center;
}

// --------------------------------------------------------------
void PhongMaterial::getReflectedRay(vec3 &hit_point, vec3 &in_ray, Ray &out_ray, vec3 &normal, bool force_specular)
{
	vec3 reflected_direction = in_ray - 2 * dot(in_ray, normal) * normal;

	if (isGlossy() && !force_specular) {
		// Glossy
		vec3 fuzzy_sphere_point = radomPointOnFuzzySphere(hit_point, reflected_direction);
		reflected_direction = fuzzy_sphere_point-hit_point;
	}

	out_ray = Ray(hit_point + normal * 0.001f, reflected_direction);
}

bool PhongMaterial::getRefractedRay(HitRecord &rec, vec3 &in_ray, Ray &out_ray, vec3 &normal, bool force_specular)
{
	

	float refraction_ratio = rec.front_face ? 1.0 / refractive_index : refractive_index;
	double cos_theta = dot(in_ray, normal);
	float k = 1.0f - refraction_ratio * refraction_ratio * (1.0f - cos_theta * cos_theta);

	// schlick Approx
	auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
	r0 = r0*r0;
	double approx = r0 + (1-r0) * pow(1-fabs(cos_theta), 5);

	if (k <= 0) 
	{
		return false;
	}

	vec3 out_direction = refraction_ratio * in_ray - (refraction_ratio * cos_theta + sqrt(k)) * normal;
	if (isGlossy() && !force_specular) {
		// Glossy
		vec3 fuzzy_sphere_point = radomPointOnFuzzySphere(rec.p, out_direction);
		out_direction = fuzzy_sphere_point-rec.p;
	}
	out_ray = Ray(rec.p - normal * 0.001f, out_direction);

	return true;
}

bool PhongMaterial::scatter(Ray &ray, HitRecord &rec, Ray &out_ray, double &attenuation, bool force_specular) 
{
	vec3 normal = normalize(rec.normal);
	vec3 in_ray = normalize(ray.getDirection());

	if (isTransparent()) 
	{
		if (getRefractedRay(rec, in_ray, out_ray, normal, force_specular))
		{
			attenuation = 0.9;
			return true;
		}
	} 

	attenuation = 0.2;
	getReflectedRay(rec.p, in_ray, out_ray, normal, force_specular);
	
	return true;
}

void PhongMaterial::getDiffuseReflected(HitRecord &rec, Ray &out_ray)
{
	float pdf;
  vec3 dir = random_direction_hemisphere(pdf);
  vec3 normal = normalize(rec.normal);
  out_ray =  Ray(rec.p + normal * 0.001f, orthonormalBasis(normal) * dir); // change of basis
}


glm::vec3 PhongMaterial::getReflectanceProb()
{
	glm::vec3 kd = this->kd();
	if (isDiffuse()) kd;

	return vec3(1.0f);
}


bool PhongMaterial::isSameHemiSphere(glm::vec3 &w, glm::vec3 &v)
{
	return w.z * v.z > 0;
}

// --------------------------------------------------------------
glm::vec3 PhongMaterial::f_diffuse(glm::vec3 &wi, glm::vec3 &wo)
{
	if (!isSameHemiSphere(wi, wo)) return vec3(0.0f);

	return getReflectanceProb() * invPi;
}


glm::vec3 PhongMaterial::sample_diffuse(glm::vec3 &normal, float &pdf)
{
	return orthonormalBasis(normal) * random_direction_hemisphere(pdf);
}


glm::vec3 PhongMaterial::f_perfect_reflect()
{
	return getReflectanceProb() * invPi;
}

// glm::vec3 PhongMaterial::sample_perfect_reflect(glm::vec3 &wo){
// 	return vec3(-wo.x, -wo.y, wo.z);
// }

glm::vec3 PhongMaterial::f_perfect_transmission()
{
	return vec3(0.0f);
}

// glm::vec3 PhongMaterial::sample_perfect_transmission(glm::vec3 &wo){
// 	return vec3(-wo.x, -wo.y, wo.z);
// }

// glm::vec3 PhongMaterial::f_rough_reflect()
// {
// 	return vec3(0.0f); // temp not available
// }

// glm::vec3 PhongMaterial::f_rough_transmission()
// {
// 	return vec3(0.0f); // temp not available
// }

// --------------------------------------------------------------

void PhongMaterial::sample_ray(HitRecord &rec, Ray &ray, Ray &light_ray, bool &goTrans, float &pdf)
{
	vec3 normal = normalize(rec.normal);
	vec3 eye_ray = normalize(ray.getDirection());

	if (isDiffuse()){
		light_ray = Ray(rec.p, sample_diffuse(normal, pdf));
		return;
	}

	pdf = 1.0f;
	if (isTransparent() && getRefractedRay(rec, eye_ray, light_ray, normal))
	{
		
		goTrans = true;
		return;
	}

	getReflectedRay(rec.p, eye_ray, light_ray, normal);

}

glm::vec3 PhongMaterial::evaluateBRDF(vec3 wi, vec3 wo)
{
	if (isDiffuse())
	{
		return f_diffuse(wi, wo);
	} 
	
	else if (isTransparent())
	{
		return vec3(0.0f);
	}
	else 
	{
		return vec3(1.0f);
	}
}

glm::vec3 PhongMaterial::BRDF(Ray &eye_ray, HitRecord &rec, Ray &light_ray, float &pdf)
{
	bool goTrans = false;
	sample_ray(rec, eye_ray, light_ray, goTrans, pdf);

	if (isDiffuse())
	{
		glm::vec3 wi= normalize(light_ray.getDirection());
		glm::vec3 wo= normalize(eye_ray.getDirection());
		// pdf = invPi * abs(dot(normalize(rec.normal), normalize(wo)));
		return f_diffuse(wi, wo);
	}
	else if (isTransparent())
	{
		return f_perfect_transmission();
	}
	else
	{
		return f_perfect_reflect();
	}
}
