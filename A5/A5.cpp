// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A5.hpp"
#include "HitRecord.hpp"
#include "PhongMaterial.hpp"
#include "utils.hpp"

#ifdef RENDER_MULTITHREADING
#include <vector>
#include <thread>
#endif

using namespace glm;
int SAMPLE_SIZE = 10;

vec3 ray_trace(Ray &ray, SceneNode *root, const glm::vec3 &ambient,
			   const std::list<Light *> &lights, const glm::vec3 &eye, int depth)
{
	// BASE CASE
	if (depth == 0)
	{
		return vec3(0.0f);
	}

	vec3 colour = vec3(0.0f);

	HitRecord rec;
	rec.t = std::numeric_limits<float>::max();
	rec.texture = nullptr;
	if (root->intersected(ray, 0.00001f, rec))
	{
		// get colour of the closet object
		PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);

		if (!(rec.texture || material))
			goto noIntersection;

		vec3 kd = rec.texture ? rec.texture->colour(rec.u, rec.v) : material->kd();
		vec3 ks = rec.texture ? rec.texture->colour(rec.u, rec.v) : material->ks();

		colour = ambient * kd;

		// move hitpoint outside of the surface to avoid incorrect numerical rounding
		rec.p += normalize(rec.normal) * 0.0001f;

		for (Light *light : lights)
		{
			Ray shadow = Ray(rec.p, light->position - rec.p);
			HitRecord dummyRec;
			dummyRec.t = std::numeric_limits<float>::max();

			if (root->intersected(shadow, 0.0f, dummyRec))
			{
				continue;
			}

			vec3 l = light->position - rec.p;
			float light_length = length(l);
			float attenuation = light->falloff[0] + light->falloff[1] * light_length + light->falloff[2] * light_length * light_length;

			l = normalize(l);
			vec3 n = normalize(rec.normal);
			vec3 r = normalize(-l + 2 * dot(l, n) * n);
			vec3 v = normalize(eye - rec.p);

			float vDotRPow = pow(max(0.0f, (float)dot(v, r)), material->p());
			float nDotL = dot(n, l);

			colour += light->colour * (kd + ks * vDotRPow / nDotL) * nDotL / attenuation;
		}

		// RECURSIVE CASE

		double out_atten;
		Ray out_ray;

		if (material->scatter(ray, rec, out_ray, out_atten))
		{
			// std::cout <<"reflected "<<out_atten<<std::endl;
			colour *= (1 - out_atten);
			colour += out_atten * ray_trace(out_ray, root, ambient, lights, eye, depth - 1);
		}

		return colour;
	}
noIntersection:
{
	// use ray direction
	vec3 direc = normalize(ray.getDirection());
	// up blue down orange
	colour = (1 - direc.y) * vec3(1.0, 0.702, 0.388) + (direc.y) * vec3(0.357, 0.675, 0.831);

	// colour = (1 - direc.y) * vec3(0, 0.153, 0.878) + (direc.y) * vec3(0, 0.027, 0.161);
}
	return colour;
}


void traceColorPerRow(SceneNode *root, Image *image, const glm::vec3 &eye,
					  uint j, size_t h, size_t w,
					  const glm::vec3 &x, const glm::vec3 &y, const glm::vec3 &bot_left_direction,
					  const glm::vec3 &ambient, const std::list<Light *> &lights)
{
	for (uint i = 0; i < w; i++)
	{
		// get the direction to every pixel
		vec3 direction = bot_left_direction + (float)(h - j) * y + (float)i * x;

		vec3 colour;

#ifdef RENDER_RECURSIVE_RAY_TRACING
		int depth = 5;
#else
		int depth = 1;
#endif

#ifdef RENDER_ANTI_ALIASING
		for (int i = 0; i < SAMPLE_SIZE; ++i)
		{
			vec3 random_p = random_in_unit_disk();
			Ray ray = Ray(eye, direction + 0.5 * random_p.x * x + 0.5 * random_p.y * y);
			colour += ray_trace(ray, root, ambient, lights, eye, depth);
		}
		colour /= SAMPLE_SIZE;
#else
		Ray ray = Ray(eye, direction);
		colour = ray_trace(ray, root, ambient, lights, eye, depth);
#endif

		// Red:
		image->operator()(i, j, 0) = (double)colour.r;
		// Green:
		image->operator()(i, j, 1) = (double)colour.g;
		// Blue:
		image->operator()(i, j, 2) = (double)colour.b;
	}
}

void A5_Render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height
	Image &image,

	// Viewing parameters
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double fovy,

	// Lighting parameters
	const glm::vec3 &ambient,
	const std::list<Light *> &lights)
{

	// Fill in raytracing code here...

	std::cout << "F23: Calling A5_Render(\n"
			  << "\t" << *root << "\t"
			  << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
																					 "\t"
			  << "eye:  " << glm::to_string(eye) << std::endl
			  << "\t"
			  << "view: " << glm::to_string(view) << std::endl
			  << "\t"
			  << "up:   " << glm::to_string(up) << std::endl
			  << "\t"
			  << "fovy: " << fovy << std::endl
			  << "\t"
			  << "ambient: " << glm::to_string(ambient) << std::endl
			  << "\t"
			  << "lights{" << std::endl;

	for (const Light *light : lights)
	{
		std::cout << "\t\t" << *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std::cout << ")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	// Get direction of the ray

	// calculate unit vector of x, y, z
	vec3 z = normalize(view);
	vec3 x = cross(z, up);
	vec3 y = normalize(up);

	// calculate the distance from eye to the center of screen
	float d_norm = h / 2 / tan(radians(fovy / 2));
	// get the vector pointing to bottom left corner of the screen
	vec3 bot_left_direction = z * d_norm - x * (w / 2) - y * (h / 2);

#ifdef RENDER_MULTITHREADING
	std::vector<std::thread> threads;
#endif
	// colouring every pixel in the screen
	for (uint j = 0; j < h; j++)
	{
#ifdef RENDER_MULTITHREADING
		threads.push_back(std::thread(traceColorPerRow, root, &image,
									eye, j, h, w, x, y, bot_left_direction,
									ambient, lights));
#else
		traceColorPerRow(root, image, eye, j, h, w, x, y, bot_left_direction,
						 ambient, lights);
#endif
	}

#ifdef RENDER_MULTITHREADING
	for (auto &th : threads)
	{
		th.join();
	}
#endif	
}
