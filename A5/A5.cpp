// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A5.hpp"
#include "HitRecord.hpp"
#include "PhongMaterial.hpp"
#include "utils.hpp"
#include "PathTracer.hpp"
#include "PhotonMapper.hpp"

#ifdef RENDER_MULTITHREADING
#include <vector>
#include <thread>
#endif

using namespace glm;
int SAMPLE_SIZE = 10;

void echoProgress(int now, int total)
{
	std::cout << "\b\b\b\b\b\b\b\b";
	std::cout << std::to_string(now * 100 / total) << "\% DONE";
	std::cout << std::flush;
}

void traceColorPerRow(SceneNode *root, Integrator *inte, Image *image, const glm::vec3 &eye,
					  uint j, size_t h, size_t w,
					  const glm::vec3 &x, const glm::vec3 &y, const glm::vec3 &bot_left_direction,
					  const glm::vec3 &ambient, const std::list<Light *> &lights)
{
	initRNG();
	// const std::list<Light *> lights;
	// PhotonMapper* p_mapper;
	for (uint i = 0; i < w; i++)
	{
		// get the direction to every pixel
		vec3 direction = bot_left_direction + (float)(h - j) * y + (float)i * x;
		// std::cout << to_string(direction ) << std::endl;
		vec3 colour = vec3(0.0f);

#ifdef RENDER_RECURSIVE_RAY_TRACING
		int depth = 5;
#else
		int depth = 1;
#endif

#ifdef RENDER_ANTI_ALIASING
		for (int k = 0; k < SAMPLE_SIZE; ++k)
		{
			vec3 random_p = random_in_unit_disk();
			Ray ray = Ray(eye, direction + 0.5 * random_p.x * x + 0.5 * random_p.y * y);
			colour += inte->ray_trace(ray, root, ambient, lights, eye, depth);
		}
		colour /= SAMPLE_SIZE;
#else
		Ray ray = Ray(eye, direction);
		colour = inte->ray_trace(ray, root, ambient, lights, eye, depth);
		// if (inte->produceImage(ray)){
		// 	// std::cout << "hit " << i << ", " << j << std::endl;
		// 	colour = vec3(1.0f, 1.0f, 1.0f);
		// }
#endif

		// Red:
		image->operator()(i, j, 0) = (double)colour.r;
		// Green:
		image->operator()(i, j, 1) = (double)colour.g;
		// Blue:
		image->operator()(i, j, 2) = (double)colour.b;
	}
	delRNG();
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
	#ifdef RENDER_AREA_LIGHT
	std::list<AreaLight *> a_lights;
	root->collect_lights(a_lights);

	for (auto *l : a_lights)
	{
		root->lights_vec.push_back(l);
	}
	const std::list<Light *> c_lights;
	for (auto *l : a_lights){
		c_lights.push_back(l);
	}
	#else
	const std::list<Light *> c_lights = lights;
	#endif

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

	

	// for (const Light *light : lights)
	// {
	// 	std::cout << "\t\t" << *light << std::endl;
	// }
	// std::cout << "\t}" << std::endl;
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

	Image *im_ptr = &image;

#ifdef RENDER_MULTITHREADING
	std::vector<std::thread> threads;
	uint progress = 0;
#endif

	PhotonMapper *pMapper = new PhotonMapper();
	pMapper->emitPhotons(root, c_lights);

	// PathTracer *pTracer =  new PathTracer();

	// colouring every pixel in the screen
	for (uint j = 0; j < h; j++)
	{

#ifdef RENDER_MULTITHREADING
		threads.push_back(std::thread([root, pMapper, im_ptr,
		eye, j, h, w, x, y, bot_left_direction,
		ambient,c_lights]()
		{
			traceColorPerRow(root, pMapper,  im_ptr,
									  eye, j, h, w, x, y, bot_left_direction,
									  ambient, c_lights);
		}));
#else
		traceColorPerRow(root, pMapper, &image, eye, j, h, w, x, y, bot_left_direction,
						 ambient, c_lights);
						//  break;
#endif
	}

#ifdef RENDER_MULTITHREADING
	for (auto &th : threads)
	{
		th.join();
		progress++;
		echoProgress(progress, h);
	}
#endif
}
