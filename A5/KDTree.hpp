#pragma once

#include <vector>
#include <queue>

#include <glm/glm.hpp>
#include "Photon.hpp"


struct KDNode
{
    // glm::vec3 pt;
    int axis; // 0 -> x, 1 -> y, 2 -> z
    int idx; // idx in the photons vector
    KDNode* left;
    KDNode* right;

    KDNode()
    : axis(-1), idx(-1), left(nullptr), right(nullptr) {}

    KDNode(int _axis, int _idx)
    : axis(_axis), idx(_idx), left(nullptr), right(nullptr) {}

    KDNode(int _axis, int _idx, KDNode *_left, KDNode *_right)
    : axis(_axis), idx(_idx), left(_left), right(_right) {}

};

class KDTree{
    public:
    std::vector<Photon> photons;
    KDNode *root;
    size_t size;

    KDTree();
    ~KDTree();
    void init(std::vector<Photon> &_photons);
    const Photon *getPhotonByIndex(int idx);
    size_t get_size();
    void kNeareastNeighbours(glm::vec3 position, float &r, std::vector<int> &nearestPhotonIdx, size_t max_size,float max_dist);

    private:
    void kNeareastNeighboursHelper(glm::vec3 position, KDNode *node, size_t max_size, float max_dist,
        std::priority_queue<std::pair<float, int>> &found_photons); 
    KDNode* initNodes(int *node_iter_start, int iter_range, int dim);
    void delNode(KDNode *n);
};