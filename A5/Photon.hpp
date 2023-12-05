# pragma once
#include <glm/glm.hpp>

struct Photon
{
    glm::vec3 position;
    glm::vec3 power; // TODO:
    // char phi;
    // char theta;
    glm::vec3 incident_dir;
    short flag;

    Photon(glm::vec3 _position, glm::vec3 _power, glm::vec3 _incident_dir)
    : position(_position), power(_power), incident_dir(_incident_dir) // TODO: might change later
    {}
};