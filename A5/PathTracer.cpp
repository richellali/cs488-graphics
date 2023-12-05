#include "PathTracer.hpp"

#include <glm/ext.hpp>

#include "HitRecord.hpp"
#include "PhongMaterial.hpp"
#include "utils.hpp"

using namespace glm;

vec3 PathTracer::ray_trace(Ray &ray, SceneNode *root, const glm::vec3 &ambient,
						   const std::list<Light *> &lights, const glm::vec3 &eye, int depth, const Texture *scene_text)
{
	if (depth == 0)
	{
		return vec3(0.0f);
	}

	vec3 colour = vec3(0.0f);

	HitRecord rec;
	if (root->intersected(ray, 0.00001f, rec))
	{
		PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);

		if (material->isEmissive())
			return material->ke();
		// if (!(rec.texture || material))
		// 	goto noIntersection;

		vec3 kd = material->kd(rec.uv.x, rec.uv.y);
		vec3 ks = material->ks(rec.uv.x, rec.uv.y);

		// std::cout << "intersected" << std::endl;
		colour = ambient * kd;

		for (Light *light : lights)
		{
#ifdef RENDER_SOFT
			int n_sample = 200;
			for (int i = 0; i < n_sample; i++)
			{
				vec3 light_pos = light->getRandomPoint();
#else
			vec3 light_pos = light->position;
			// std::cout << to_string(light_pos) << std::endl;
#endif
				// move hitpoint outside of the surface to avoid incorrect numerical rounding
				Ray shadow = Ray(rec.p + normalize(rec.normal) * 0.001f, light_pos - rec.p);
				HitRecord dummyRec;
				if (root->intersected(shadow, 0.0f, dummyRec))
				{
					continue;
				}

				vec3 l = light_pos - rec.p;
				float light_length = length(l);
				float attenuation = light->falloff[0] + light->falloff[1] * light_length + light->falloff[2] * light_length * light_length;

				l = normalize(l);
				vec3 n = normalize(rec.normal);
				vec3 r = normalize(-l + 2 * dot(l, n) * n);
				vec3 v = normalize(eye - rec.p);

				float vDotRPow = pow(max(0.0f, (float)dot(v, r)), material->p());
				float nDotL = dot(n, l);

				// std::cout << nDotL << ", " << vDotRPow << std::endl;

#ifdef RENDER_SOFT
				colour += light->colour * (kd + ks * vDotRPow / nDotL) * nDotL / attenuation / n_sample;
			}
#else
			colour += light->colour * (kd + ks * vDotRPow / nDotL) * nDotL / attenuation;
#endif
		}
		std::cout << to_string(colour) << std::endl;

		// RECURSIVE CASE

		double out_atten;
		Ray out_ray;
		// std::cout << "scatter" << std::endl;
		if (!material->isDiffuse() && material->scatter(ray, rec, out_ray, out_atten))
		{
			colour *= (1 - out_atten);
			colour += out_atten * ray_trace(out_ray, root, ambient, lights, eye, depth - 1, scene_text);
		}
		// std::cout << "scatter back" << std::endl;
		return colour;
	}

noIntersection:
{
	// use ray direction
	// std::cout << "no Intersection " << std::endl;
	vec3 direc = normalize(ray.getDirection());
	// up blue down orange
	colour = (1 - direc.y) * vec3(1.0, 0.702, 0.388) + (direc.y) * vec3(0.357, 0.675, 0.831);

	// colour = (1 - direc.y) * vec3(0, 0.153, 0.878) + (direc.y) * vec3(0, 0.027, 0.161);

}
	return colour;
}
