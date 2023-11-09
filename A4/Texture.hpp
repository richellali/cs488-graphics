
#pragma once

#include <glm/glm.hpp>
#include <lodepng/lodepng.h>
#include <vector>
#include <string>

typedef unsigned int uint;

class Texture {
    public:
    Texture(const std::string& fname );
    glm::vec3 colour(double u, double v);
    std::string fname;

    private:
    uint m_width;
	uint m_height;
	std::vector<unsigned char> image;
};


