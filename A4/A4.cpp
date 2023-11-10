// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A4.hpp"
#include "HitRecord.hpp"
#include "PhongMaterial.hpp"

using namespace glm;

vec3 ray_trace(Ray &ray, SceneNode *root, const glm::vec3 & ambient,
		const std::list<Light *> & lights, const glm::vec3 & eye) {
	vec3 colour;

	HitRecord rec;
	rec.t = std::numeric_limits<float>::max();
	rec.texture = nullptr;
	if (root->intersected(ray, 0.00001f, rec)){
		// get colour of the closet object
		PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);

		if (!(rec.texture || material)) goto noIntersection;
		vec3 kd = rec.texture ? rec.texture->colour(rec.u, rec.v) : material->kd();
		vec3 ks = rec.texture ? rec.texture->colour(rec.u, rec.v) : material->ks();

		// if (rec.texture) std::cout << to_string(rec.texture->colour(rec.u, rec.v)) << std::endl;

		// ambient light source
		// if (rec.texture) {
		// 	colour = ambient * kd;
		// 	return colour;
		// }
		colour = ambient * kd;

		// move hitpoint outside of the surface to avoid incorrect numerical rounding
		rec.p += normalize(rec.normal) * 0.0001f;

		for (Light *light : lights) {
			Ray shadow = Ray(rec.p, light->position - rec.p);
			HitRecord dummyRec;
			dummyRec.t = std::numeric_limits<float>::max();

			if (root->intersected(shadow, 0.0f, dummyRec)){
				continue;
			}
			
			vec3 l = light->position - rec.p;
			float light_length = length(l);
			float attenuation = light->falloff[0] + light->falloff[1]*light_length + light->falloff[2]*light_length*light_length;
			
			l = normalize(l);
			vec3 n = normalize(rec.normal);
			vec3 r = normalize(-l + 2 * dot(l, n) * n);
			vec3 v = normalize(eye - rec.p);

			float vDotRPow = pow(max(0.0f, (float)dot(v, r)), material->p());
			float nDotL = dot(n, l);
			
			colour += light->colour * (kd + ks * vDotRPow / nDotL) * nDotL / attenuation;
		}
		return colour;
	} 
	noIntersection:
	{
		// use ray direction 
		vec3 direc = normalize(ray.getDirection());
		// up blue down orange
		colour = (1-direc.y) * vec3(1.0, 0.702, 0.388)+ (direc.y) * vec3(0.357, 0.675, 0.831);

	}
	return colour;
}

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  std::cout << "F23: Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

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
	vec3 bot_left_direction = z * d_norm - x * (w/2) - y * (h/2);

	// colouring every pixel in the screen
	for (uint j=0; j<h; j++){
		for (uint i=0; i<w; i++){
			// get the direction to every pixel
			vec3 direction = bot_left_direction + (float)(h-j) * y + (float)i * x;

			Ray ray = Ray(eye, direction);

			vec3 colour= ray_trace(ray, root, ambient, lights, eye);

			// Red: 
			image(i, j, 0) = (double)colour.r;
			// Green: 
			image(i, j, 1) = (double)colour.g;
			// Blue: 
			image(i, j, 2) = (double)colour.b;
		}
	}



}
