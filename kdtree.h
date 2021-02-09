#pragma once
#ifndef KDTREE_H
#define KDTREE_H

#include "common.h"

#include <vector>

class KDTree
{
private:
    Node* _root;
    unsigned int _depth;

public:
    KDTree (std::vector<Point*> &points);
    //KDTree (std::vector<Point*> points);
    //KDTree (Point *points, uint32_t size);
    //KDTree (Point points[], uint32_t size);

    void print ();

//protected:
    ~KDTree ()
    {
        delete _root;
    }

private:
    Node* build (std::vector<Point*> &points, unsigned int depth);

    void printNode (Node* node);
};
#endif // KDTREE_H