#include "PhotonMapper.hpp"
#include "utils.hpp"
#include "polyroots.hpp"

#include <glm/gtx/component_wise.hpp>
#include <glm/ext.hpp>
#include <thread>

extern const float Pi;
using namespace glm;

// int GLOBAL_PHOTON_COUNT = 1000000;
int GLOBAL_PHOTON_COUNT = 5000;     // Number of photons emmitted for global map
int CAUSTIC_PHOTON_COUNT = 1000000; // Number of photons emmited for caustic map
int MAX_PHOTON_DEPTH = 20;

// Photon Map Sampling Parameters
int INDIRECT_TEST = 256;
int GLOBAL_ESTIMATE_SIZE = 50;
float GLOBAL_ESTIMATE_DIST = 20;
int CAUSTIC_ESTIMATE_SIZE = 100;
float CAUSTIC_ESTIMATE_DIST = 2.5;

int LIGHT_SAMPLES = 50;
int SHADOW_TEST = 128;
int SPECULAR_TEST = 128;

PhotonMapper::PhotonMapper()
    : causticPhotonMap(), globalPhotonMap(),
      globalPhotonsThread(), causticPhotonsThread(),
      globalPhotons(), causticPhotons()
{
}

PhotonMapper::~PhotonMapper()
{
}

// TODO:: FIX AREALIGHT Later
void PhotonMapper::emitPhotons(SceneNode *root, const std::list<Light *> &lights)
{
    std::cout << "Emit Photons" << std::endl;
    size_t total_global_photons = GLOBAL_PHOTON_COUNT;
    size_t total_caustic_photons = CAUSTIC_PHOTON_COUNT;

    int per_thread_max_global_photons = 2000;
    int per_thread_max_caustic_photons = 2000;

    double total_light_power = 0.0;

    for (auto l : lights)
    {
        total_light_power += compAdd(l->colour) * l->getArea();
    }

    struct ThreadWork
    {
        vec3 photon_power;
        size_t num_photons;
        // MapType type;
        Light *l;

        ThreadWork(vec3 _photon_power, size_t _num_photons, Light *_l)
            : photon_power(_photon_power), num_photons(_num_photons), l(_l) {}
    };

    std::vector<ThreadWork> globalThreadWorks;
    std::vector<ThreadWork> causticThreadWorks;

    for (Light *l : lights)
    {
        // // More photons should be emitted from brighter lights
        double per_light_photons_ratio = compAdd(l->getPower()) / total_light_power;
        size_t cur_light_photons;

        // // global
        // size_t per_light_global_photons_num = per_light_photons_ratio * total_global_photons;
        // // size_t per_light_global_photons_num =
        // // determine power of each photon
        // vec3 global_photon_power = vec3(0.001, 0.001, 0.001);
        // std::cout << "global photon power: "<<  to_string(global_photon_power) << std::endl;

        // cur_light_photons = 0;

        // while (cur_light_photons < per_light_global_photons_num)
        // {
        //     size_t cur_thread_photons =
        //         cur_light_photons + per_thread_max_global_photons <= per_light_global_photons_num
        //             ? per_thread_max_global_photons
        //             : per_light_global_photons_num - per_thread_max_global_photons;

        //     globalThreadWorks.push_back(ThreadWork(global_photon_power, cur_thread_photons, l));

        //     cur_light_photons += cur_thread_photons;
        // }

        // caustic
        size_t per_light_caustic_photons_num = per_light_photons_ratio * total_caustic_photons;
        vec3 caustic_photon_power = vec3(0.01, 0.01, 0.01);

        std::cout << "cuastic photon power: " << to_string(caustic_photon_power) << std::endl;

        cur_light_photons = 0;

        while (cur_light_photons < per_light_caustic_photons_num)
        {
            size_t cur_thread_photons =
                cur_light_photons + per_thread_max_caustic_photons <= per_light_caustic_photons_num
                    ? per_thread_max_caustic_photons
                    : per_light_caustic_photons_num - per_thread_max_caustic_photons;

            causticThreadWorks.push_back(ThreadWork(caustic_photon_power, cur_thread_photons, l));

            cur_light_photons += cur_thread_photons;
        }
    }

    std::vector<std::unique_ptr<std::thread>> g_threads(globalThreadWorks.size());
    globalPhotonsThread.resize(globalThreadWorks.size());

    for (int i = 0; i < globalThreadWorks.size(); ++i)
    {
        ThreadWork tw = globalThreadWorks[i];
        g_threads[i] = std::make_unique<std::thread>([this, &tw, root, i]()
                                                     { this->tracePhotonsPerThread(root, tw.l, tw.photon_power, tw.num_photons, MapType::GLOBAL, i); });
    }

    for (auto &th : g_threads)
    {
        th->join();
    }

    std::cout << globalThreadWorks.size() << " global done" << std::endl;

    std::vector<std::unique_ptr<std::thread>> c_threads(causticThreadWorks.size());
    causticPhotonsThread.resize(causticThreadWorks.size());

    for (int i = 0; i < causticThreadWorks.size(); ++i)
    {
        ThreadWork tw = causticThreadWorks[i];
        c_threads[i] = std::make_unique<std::thread>([this, &tw, root, i]()
                                                     { this->tracePhotonsPerThread(root, tw.l, tw.photon_power, tw.num_photons, MapType::CAUSTIC, i); });
    }

    for (auto &th : c_threads)
    {
        th->join();
    }

    std::cout << causticThreadWorks.size() << " caustic done" << std::endl;

    std::vector<std::unique_ptr<std::thread>> treethreads;

    // Merge Photons
    // treethreads.push_back(std::make_unique<std::thread>([this]()
    //                                                     { this->MergePhotonsAndBuildTree(MapType::GLOBAL); }));
    treethreads.push_back(std::make_unique<std::thread>([this]()
                                                        { this->MergePhotonsAndBuildTree(MapType::CAUSTIC); }));
    for (auto &th : treethreads)
    {
        th->join();
    }
    // MergePhotonsAndBuildTree(MapType::CAUSTIC);

    // Clear all vectors bc we will never use them
    globalPhotonsThread.clear();
    globalPhotons.clear();

    causticPhotonsThread.clear();
    causticPhotons.clear();
}

void PhotonMapper::MergePhotonsAndBuildTree(MapType type)
{
    size_t ttl_size = 0;

    if (type == MapType::GLOBAL)
    {
        for (auto &th : globalPhotonsThread)
        {
            ttl_size += th.size();
        }
        globalPhotons.reserve(ttl_size);

        for (auto &th : globalPhotonsThread)
        {
            globalPhotons.insert(globalPhotons.end(), th.begin(), th.end());
        }
        globalPhotonMap.init(globalPhotons);
    }
    else
    {
        for (auto &th : causticPhotonsThread)
        {
            ttl_size += th.size();
        }
        causticPhotons.reserve(ttl_size);
        for (auto &th : causticPhotonsThread)
        {
            causticPhotons.insert(causticPhotons.end(), th.begin(), th.end());
        }
        causticPhotonMap.init(causticPhotons);
    }
}

void PhotonMapper::tracePhotonsPerThread(SceneNode *root, Light *l,
                                         vec3 photon_power, size_t max_photons, MapType type, int thread_idx)
{
    initRNG();
    int max_depth = MAX_PHOTON_DEPTH;
    for (int i = 0; i < max_photons; i++)
    {
        vec3 pt, dir;
        l->getRandomPointAndDirection(pt, dir);
        Ray ray(pt, dir);
        tracePhotons(ray, root, photon_power, max_depth, type, thread_idx, false);
    }
    delRNG();
}

void PhotonMapper::tracePhotonsPerThread(SceneNode *root, AreaLight *l,
                                         vec3 photon_power, size_t max_photons, MapType type, int thread_idx)
{
    initRNG();
    int max_depth = MAX_PHOTON_DEPTH;
    for (int i = 0; i < max_photons; i++)
    {
        vec3 pt, dir;
        l->getRandomPointAndDirection(pt, dir);
        Ray ray(pt, dir);
        tracePhotons(ray, root, photon_power, max_depth, type, thread_idx, false);
    }
    delRNG();
}

void PhotonMapper::tracePhotons(Ray &ray, SceneNode *root, vec3 photon_power, int max_depth, MapType type, int thread_idx, bool prev_specular)
{
    // caustic maps only contain photons hit a specular surface
    if (max_depth == 0)
        return;

    HitRecord rec;
    // std::cout << "before hit" << std::endl;
    if (root->intersected(ray, 0.00001f, rec))
    {
        // std::cout << "hit" << std::endl;
        PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);

        if (material == nullptr) std::cout << "no mat" << std::endl;

        if (material->isDiffuse())
        {
            // emplace in global photon map
            if (type == MapType::GLOBAL)
            {
                globalPhotonsThread[thread_idx].push_back(Photon(rec.p, photon_power, -ray.getDirection()));
            }

            // also add to Caustic map if it has hit specular
            if (prev_specular && type == MapType::CAUSTIC)
            {
                causticPhotonsThread[thread_idx].push_back(Photon(rec.p, photon_power, -ray.getDirection()));
            }
            prev_specular = false;
        }

        // set flag -> if it survives in Russian Roulete add it to Caustic Map
        prev_specular = material->isReflected() || material->isTransparent();

        // Russian Roulete
        glm::vec3 kd = material->kd();
        if (!material->isDiffuse() && type == MapType::CAUSTIC)
            kd = vec3(0.0f);
        glm::vec3 ks = material->ks();

        float Pd, Ps, Pa;
        russian_roulette_prob(kd, ks, Pd, Ps, Pa);

        float Pt = max(Pd + Ps + Pa, 1.0f);

        float rand_res = random_double() * Pt;
        Ray out_ray;

        if (rand_res <= Pd)
        { // diffuse
            photon_power = photon_power * kd / (Pd / Pt);
            glm::vec3 normal = normalize(rec.normal);
            float pdf;
            vec3 out_dir = material->sample_diffuse(normal, pdf);
            out_ray = Ray(rec.p + 0.001f * normal, out_dir);
        }
        else if (rand_res <= Pd + Ps)
        {
            // std::cout << "specular" << std::endl;
            if (type != MapType::CAUSTIC)
                return;
            double out_atten;
            photon_power = photon_power * ks / (Ps / Pt);

            material->scatter(ray, rec, out_ray, out_atten, true); // will decide if it will reflect or trans
        }
        else
        {
            return;
        }

        return tracePhotons(out_ray, root, photon_power, max_depth - 1, type, thread_idx, prev_specular);
    }
}

glm::vec3 PhotonMapper::radiance_estimate(glm::vec3 &pos, glm::vec3 view, glm::vec3 normal, float cos_theta, glm::vec3 out_dir,
                                          PhongMaterial *material, MapType type)
{
    size_t max_size;
    float max_dist;

    KDTree *photonMap;
    if (type == MapType::GLOBAL)
    {
        photonMap = &globalPhotonMap;
        max_size = GLOBAL_ESTIMATE_SIZE;
        max_dist = GLOBAL_ESTIMATE_DIST;
    }
    else
    {
        photonMap = &causticPhotonMap;
        max_size = CAUSTIC_ESTIMATE_SIZE;
        max_dist = CAUSTIC_ESTIMATE_DIST;
    }
    std::vector<int> nearestPhotonIdx;
    float r;
    photonMap->kNeareastNeighbours(pos, r, nearestPhotonIdx, max_size, max_dist);

    if (nearestPhotonIdx.empty())
        return vec3(0.0f);

    glm::vec3 total_flux = vec3(0.0f);
    for (int p_idx : nearestPhotonIdx)
    {
        const Photon *p_ptr = photonMap->getPhotonByIndex(p_idx);

        glm::vec3 estimate = p_ptr->power;
        vec3 incident = normalize(p_ptr->incident_dir);

        float nDotL = dot(normal, incident);
        float vDotRPow = pow(max(0.0f, (float)dot(out_dir, -view)), material->p());

        estimate *= nDotL * material->kd() + vDotRPow * material->ks();

        total_flux += estimate;
    }

    total_flux /= Pi * r * r;

    return total_flux;
}

glm::vec3 PhotonMapper::computeIndirectIllumination(SceneNode *root, glm::vec3 &pos, glm::vec3 &normal,
                                                    PhongMaterial *material, float cos_theta, bool inMonteCarlo)
{
    if (!material->isDiffuse())
        return vec3(0.0f);
    // Scale number of samples with contribution to final color of pixel
    vec3 kd = material->kd();
    float highest_weight = max(kd.x, kd.y, kd.z);
    int num_samples = ceil((INDIRECT_TEST * highest_weight + INDIRECT_TEST) / 2.0);
    if (inMonteCarlo)
        num_samples = 1;

    // Recursively raytrace the bounce using montecarlo path tracing
    vec3 colour_buffer = vec3(0.0f);
    Ray ray;
    vec3 sampled_bounce;
    for (int i = 0; i < num_samples; i++)
    {
        // Diffuse importance sample
        float pdf;
        sampled_bounce = material->sample_diffuse(normal, pdf);
        ray = Ray(pos + sampled_bounce * 0.001f, sampled_bounce);
        colour_buffer += MonteCarlo_IndirectSample(root, ray);
    }
    // Normalize average and add contribution
    return colour_buffer / num_samples * kd;
}

glm::vec3 PhotonMapper::MonteCarlo_IndirectSample(SceneNode *root, Ray &ray)
{

    vec3 intensity = vec3(1.0f);
    vec3 colour = vec3(0.0f);
    // Bounce until diffuse interaction
    for (int iter = 0; iter < 10; iter++)
    {
        HitRecord rec;
        if (root->intersected(ray, 0.00001f, rec))
        {
            // Get intersection information
            PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);

            // Color buffer (compute and scale once)
            vec3 colour_buffer = vec3(0.0f);

            // Useful geometric values to precompute
            vec3 v = normalize(rec.p - ray.getOrigin());
            vec3 n = normalize(rec.normal);
            float cos_theta = dot(n, -v);

            // Compute Reflection Coefficient, carry reflection portion to Specular

            vec3 kd = material->kd();

            vec3 ks = material->ks();

            float Pd, Ps, Pa;
            russian_roulette_prob(kd, ks, Pd, Ps, Pa);

            float Pt = max(Pd + Ps + Pa, 1.0f);

            float rand = random_double();
            if (Pt > 1.0)
            {
                rand *= Pt;
            }
            
            Ray out_ray; // TODO::
            // Bounce and sample or recur (choose one from distribution)
            if (rand < Pd)
            {
                material->getReflectedRay(rec.p, v, out_ray, n);
                vec3 out_dir = normalize(out_ray.getDirection());
                colour_buffer += radiance_estimate(rec.p, v, n, cos_theta, out_dir,
                                                   material, MapType::GLOBAL);
                colour += colour_buffer * kd * intensity / Pd;
                break;
            }
            else if (rand < Pd + Ps)
            {
                
                double out_atten;
                material->scatter(ray, rec, out_ray, out_atten, true); // will decide if it will reflect or trans
                intensity *= ks / Ps;
            }
            else
            {
                // Photon absorbed; terminate trace
                break;
            }
            ray = out_ray;
        }
        else
        {
            // Intersect with background and break
            vec3 direc = normalize(ray.getDirection());
            // up blue down orange
            vec3 bg = (1 - direc.y) * vec3(1.0, 0.702, 0.388) + (direc.y) * vec3(0.357, 0.675, 0.831);

            // colour = (1 - direc.y) * vec3(0, 0.153, 0.878) + (direc.y) * vec3(0, 0.027, 0.161);
            colour += intensity * bg;
            break;
        }
    }

    return colour;
}

glm::vec3 PhotonMapper::ray_trace(Ray &ray, SceneNode *root, const glm::vec3 &ambient,
                                  const std::list<Light *> &lights, const glm::vec3 &eye, int depth)
{
    vec3 colour = vec3(0.0f);

    if (depth == 0)
        return colour;

    HitRecord rec;
    // std::cout << "before intersect" << std::endl;
    if (root->intersected(ray, 0.00001f, rec))
    {
        PhongMaterial *material = static_cast<PhongMaterial *>(rec.material);
        if (material == nullptr) std::cout << "no mat" << std::endl;
        if (material->isEmissive())
            return material->ke();

        colour += ambient;

        vec3 kd = material->kd(rec.uv.x, rec.uv.y);
        vec3 ks = material->ks(rec.uv.x, rec.uv.y);

        vec3 v = normalize(eye - rec.p);
        vec3 n = normalize(rec.normal);
        vec3 in_dir = normalize(ray.getDirection());

        float cos_theta = dot(n, -v);

        // soft shadow

        for (Light *light : lights)
        {
            for (int i = 0; i <= LIGHT_SAMPLES; i++)
            {
                // std::cout << to_string(light->position) << std::endl;
                vec3 light_pos = light->getRandomPoint();
                vec3 l = light_pos - rec.p;
                Ray shadowRay = Ray(rec.p + normalize(l) * 0.001, l);
                // std::cout << to_string(rec.p) << ", " << to_string(light_pos) << "," << to_string(l) << std::endl;
                HitRecord dummyRec;

                if (root->intersected(shadowRay, 0.0f, dummyRec))
                {
                    // std::cout << "Intersect " << rec.t << std::endl;
                    continue;
                }

                float light_length = length(l);
                float attenuation = light->falloff[0] + light->falloff[1] * light_length + light->falloff[2] * light_length * light_length;

                l = normalize(l);
                vec3 r = normalize(-l + 2 * dot(l, n) * n);

                float vDotRPow = pow(max(0.0f, (float)dot(v, r)), material->p());
                float nDotL = dot(n, l);

                // std::cout << nDotL << ", " << vDotRPow << std::endl;
                // std::cout << attenuation << std::endl;
                colour += light->colour * (kd + ks * vDotRPow / nDotL) * nDotL / attenuation ;
            }
            colour /= LIGHT_SAMPLES;

            // std::cout << to_string(colour) << std::endl;
        }

        if (material->isDiffuse())
        {
            // Global
            Ray out_ray;
            material->getReflectedRay(rec.p, v, out_ray, n);
            vec3 out_dir = normalize(out_ray.getDirection());

            // colour += radiance_estimate(rec.p, v, normal, cos_theta, out_dir, material, MapType::GLOBAL);
            colour += radiance_estimate(rec.p, v, n, cos_theta, out_dir, material, MapType::CAUSTIC);
            // if (depth == 0)
            //     colour += computeIndirectIllumination(root, rec.p, n, material, cos_theta, false);
        }
        else
        {
            // RECURSIVE CASE
            double out_atten;
            Ray out_ray;
            if (!material->isDiffuse() && material->scatter(ray, rec, out_ray, out_atten))
            {
                colour *= (1 - out_atten);
                colour += out_atten * ray_trace(out_ray, root, ambient, lights, eye, depth - 1);
            }
        }
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

// visualization

bool intersect(Ray &ray, vec3 position)
{
    vec3 center_vec = ray.getOrigin() - position;
    double A = dot(ray.getDirection(), ray.getDirection());
    double B = 2 * dot(ray.getDirection(), center_vec);
    double C = dot(center_vec, center_vec) - 0.1 * 0.1;

    double roots[2];

    size_t numRoots = quadraticRoots(A, B, C, roots);
    return numRoots > 0;
}

bool PhotonMapper::produceImage(Ray &ray)
{
    size_t sz = causticPhotonMap.get_size();
    for (int i = 0; i < sz; i++)
    {
        const Photon *p_ptr = causticPhotonMap.getPhotonByIndex(i);
        if (intersect(ray, p_ptr->position))
        {
            return true;
        }
        // std::cout << to_string(p_ptr->position) << std::endl;
    }
    return false;
}