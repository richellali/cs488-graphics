#pragma once
#include <vector>
#include "PhongMaterial.hpp"
#include "Integrator.hpp"
#include "KDTree.hpp"

// #define GLOBAL_PHOTON

enum MapType
{
    GLOBAL,
    CAUSTIC
};

class PhotonMapper : public Integrator
{
public:
    PhotonMapper();
    virtual ~PhotonMapper();
    void emitPhotons(SceneNode *root, const std::list<Light *> &lights);

    glm::vec3 radiance_estimate(glm::vec3 &pos, glm::vec3 view, glm::vec3 normal, float cos_theta, glm::vec3 out_dir, PhongMaterial *material, MapType type);

    glm::vec3 ray_trace(Ray &ray, SceneNode *root, const glm::vec3 &ambient,
                        const std::list<Light *> &lights, const glm::vec3 &eye, int depth, const Texture *scene_text) override;

    bool produceImage(Ray &ray) override;

private:
    KDTree causticPhotonMap;
    KDTree globalPhotonMap;

    std::vector<std::vector<Photon>> globalPhotonsThread;
    std::vector<std::vector<Photon>> causticPhotonsThread;

    std::vector<Photon> globalPhotons;
    std::vector<Photon> causticPhotons;

    void tracePhotonsPerThread(SceneNode *root, Light *l,
                               glm::vec3 photon_power, size_t max_photons, MapType type, int thread_idx);

    void tracePhotonsPerThread(SceneNode *root, AreaLight *l,
                               glm::vec3 photon_power, size_t max_photons, MapType type, int thread_idx);
    void tracePhotons(Ray &ray, SceneNode *root, glm::vec3 photon_power,
                      int max_depth, MapType type, int thread_idx, bool prev_specular);

    void MergePhotonsAndBuildTree(MapType type);
};