#include "Texture.hpp"


#include <iostream>

std::string TEXTURE_DIR = "texture/";

using namespace glm;

Texture::Texture(const std::string &fname)
: fname(fname)
{
    // Decode the image
    unsigned error = lodepng::decode(image, m_width, m_height, TEXTURE_DIR+fname, LCT_RGB);
    // std::cout <<"decode" << fname << std::endl;
    if (error)
    {
        std::cerr << "decoder error " << fname << std::endl;
        std::cerr << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
}

glm::vec3 Texture::colour(double u, double v) const {
    // if (!(0 <= u && u <= 1)) std::cerr << "No u" << std::endl;
    // if (!(0 <= v && v <= 1)) std::cerr << "No v" << std::endl;

    int ix = (int)(u*(m_width-1));
    int iy = (int)(v*(m_height-1));

    // std::cout << ix << "," << iy << std::endl;

    unsigned long pos = (m_width * iy + ix) * 3;


    return vec3(image[pos]/255.0, image[pos+1]/255.0, image[pos+2]/255.0);
}

