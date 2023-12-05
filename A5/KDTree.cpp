#include "KDTree.hpp"
#include <numeric>
#include <iostream>
#include <glm/ext.hpp>

using namespace glm;


KDTree::KDTree()
    : photons(), root(nullptr), size(0) {}

void KDTree::delNode(KDNode *n)
{
    if (n == nullptr) return;

    delNode(n->left);
    delNode(n->right);

    delete n;
}

KDTree::~KDTree(){
    delNode(root);
}

void KDTree::init(std::vector<Photon> &_photons)
{
    photons = _photons;
    size = _photons.size();

    std::vector<int> photon_inds(size);
    std::iota(photon_inds.begin(), photon_inds.end(), 0);

    root = initNodes(photon_inds.data(), size, 0);
}

KDNode* KDTree::initNodes(int *node_iter_start, 
    int iter_range, int dim)
{
    if (iter_range <= 0) return nullptr;

    // sort in the current axis
    std::sort(node_iter_start, node_iter_start+iter_range, 
    [&](std::size_t i, std::size_t j)
    {
        return photons[i].position[dim] < photons[j].position[dim];
    });
    int pivot = (iter_range-1) / 2;

    KDNode *node = new KDNode(dim, node_iter_start[pivot]);

    // left
    node->left = initNodes(node_iter_start, pivot, (dim+1)%3);

    // right
    node->right = initNodes(node_iter_start+pivot+1, iter_range-pivot-1, (dim+1)%3);

    return node;
}

/*
    Implementation follows Photon Book By Henrik W. Jensen
*/

void KDTree::kNeareastNeighboursHelper(glm::vec3 position, KDNode *node, size_t max_size, float max_dist, std::priority_queue<std::pair<float, int>> &found_photons)
{
    if (node == nullptr) return;

    const Photon *p = &photons[node->idx];

    float dist1 = position[node->axis] - p->position[node->axis];

    if (dist1 > 0.0)
    {
        kNeareastNeighboursHelper(position, node->right, max_size, max_dist, found_photons);

        if (!found_photons.empty() 
            && dist1 * dist1 < found_photons.top().first && dist1 * dist1 < max_dist * max_dist)
        {
            kNeareastNeighboursHelper(position, node->left, max_size, max_dist, found_photons);
        }
    }
    else 
    {
        kNeareastNeighboursHelper(position, node->left, max_size, max_dist, found_photons);

        if (!found_photons.empty() 
            && dist1 * dist1 < found_photons.top().first && dist1 * dist1 < max_dist * max_dist)
        {
            kNeareastNeighboursHelper(position, node->right, max_size, max_dist, found_photons);
        }

    }

    float dist2 = length2(position - p->position);
    if (!(dist2 < max_dist * max_dist) ) return;

    if (found_photons.size() < max_size)
    {
        found_photons.emplace(std::make_pair(dist2, node->idx));
    } 
    else if (dist2 < found_photons.top().first)
    {
        found_photons.pop();
        found_photons.emplace(std::make_pair(dist2, node->idx));
    }
}

const Photon* KDTree::getPhotonByIndex(int idx){
    return &photons[idx];
}

void KDTree::kNeareastNeighbours(glm::vec3 position, float &r, std::vector<int> &nearestPhotonIdx, size_t max_size, float max_dist)
{
    std::priority_queue<std::pair<float, int>> found_photons;
    kNeareastNeighboursHelper(position, root, max_size, max_dist, found_photons);

    r = found_photons.size() < max_size
    ? max_dist
    : found_photons.top().first;

    while (!found_photons.empty())
    {
        nearestPhotonIdx.push_back(found_photons.top().second);
        found_photons.pop();
    }
}

size_t KDTree::get_size()
{
    return size;
}