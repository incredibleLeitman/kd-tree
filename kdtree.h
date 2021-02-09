#pragma once
#ifndef KDTREE_H
#define KDTREE_H

#include "common.h"

#include <vector>

class KDTree
{
private:
    Node* _root;
    uint32_t _depth;

public:
    KDTree (std::vector<Point*> &points);
    KDTree (float *vertices, uint32_t size);

    void print ();
    Node* root ();

//protected:
    ~KDTree ()
    {
        delete _root;
    }

private:
    Node* build (std::vector<Point*> &points, uint32_t depth);
    // unused: idea was to compare array vs vector but overhead for sort and comparing every third value was to much
    //Node* build(float* points, uint32_t size, uint32_t depth);

    void printNode (Node* node);
};
#endif // KDTREE_H