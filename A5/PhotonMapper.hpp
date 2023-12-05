#pragma once
#include <vector>
#include "PhongMaterial.hpp"
#include "Integrator.hpp"
#include "KDTree.hpp"

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
                        const std::list<Light *> &lights, const glm::vec3 &eye, int depth) override;

    bool produceImage(Ray &ray) override;
    glm::vec3 computeIndirectIllumination(SceneNode *root, glm::vec3 &pos, glm::vec3 &normal,
                                          PhongMaterial *material, float cos_theta, bool inMonteCarlo);
    glm::vec3 MonteCarlo_IndirectSample(SceneNode *root, Ray &ray);

    glm::vec3 MonteCarlo_PathTrace(SceneNode *root, Ray &ray);
    glm::vec3 computeSpecularIllumination(SceneNode *root, Ray &in_ray,
                                          HitRecord &rec, PhongMaterial *material, glm::vec3 &view, float cos_theta);

    glm::vec3 computeDirectIllum(SceneNode *root, HitRecord &rec, Ray &ray);

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