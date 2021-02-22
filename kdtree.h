#pragma once
#ifndef KDTREE_H
#define KDTREE_H

#include "common.h"

#include <vector>

class KDTree
{
private:
    Node* _root;
    uint32_t _nodes = 0; // total amount of nodes
    uint32_t _depth = 0; // maximum depth
    uint32_t _iterated = 0; // count of iterated nodes for raycast
    #if defined(USE_CACHE)
        uint8_t _rayId = 0; // increase raycast id each check to allow caching
    #endif

public:
    KDTree (float *vertices, size_t size);

    void print ();
    Node* root ();

    const Triangle* raycast (const Ray ray);
    const Triangle* bruteforce (const Ray ray) const;

//protected:
    ~KDTree ()
    {
        delete _root;
    }

private:
    Node* build (std::vector<Point*> &points, uint32_t depth);
    // unused: idea was to compare array vs vector but overhead for sort and comparing every third value was to much
    //Node* build(float* points, uint32_t size, uint32_t depth);

    void printNode (Node& node);
    void raycastNode (Node* node, const Triangle*& nearest_triangle, glm::vec3& result, const Ray& ray, float& nearest);
};
#endif // KDTREE_H