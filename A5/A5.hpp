// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Texture.hpp"

// #define RENDER_ANTI_ALIASING
#define RENDER_RECURSIVE_RAY_TRACING
#define RENDER_MULTITHREADING
#define RENDER_PHOTON_MAP
// #define RENDER_AREA_LIGHT

void A5_Render(
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
);
