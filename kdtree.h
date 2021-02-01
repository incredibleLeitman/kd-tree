#pragma once
#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

// ----------------------------------------
// Considerations...
// ----------------------------------------
// - using seperate ctor an build method
//      + fast ctor, manual init
//      - need of additional memory to copy points
// - array vs vector
//      + could be faster --> test, needed?
// - check if chrono clock in finally block is slower than execution
// - fill kdtree with 3d points or triangles?
// - better replace enum with int -> easier iteration, stringification and extension

enum Axis
{
    X,
    Y,
    Z
};
// C++17
//inline static const Axis Dimensions[] = { X, Y, Z };
//static const Axis Dimensions[] = { X, Y, Z };

// stores 3d points
struct Point
{
    float x;
    float y;
    float z;

    Point (float i_x, float i_y, float i_z) : x(i_x), y(i_y), z(i_z)
    {
    }

    float dim (Axis axis)
    {
        if (axis == Axis::X)
        {
            return x;
        }
        else if (axis == Axis::Y)
        {
            return y;
        }
        else //if (axis == Axis::Z)
        {
            return z;
        }
    }
};

// represent a node of the tree
struct Node
{
    Axis axis;
    Point* pt;
    Node* left;
    Node* right;

/*
public:
    Node ()
    {

    }
*/

    ~Node ()
    {
        delete left;
        delete right;
    }
};

class KDTree
{
private:
    Node* _root;

public:
    KDTree (std::vector<Point*> points);
    //KDTree (std::vector<Point*> points);
    //KDTree (Point *points, uint32_t size);
    //KDTree (Point points[], uint32_t size);

//protected:
    ~KDTree ()
    {
        delete _root;
    }

private:
    Node* build (std::vector<Point*> points);
};
#endif // KDTREE_H