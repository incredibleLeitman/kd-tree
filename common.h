#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <string>

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

// TODO:
// - outsource enums, structs and classes into own files
// - using references from lists --> better arrays
// - use float array for point

#define SEPERATE_MIN_MAX        // use seperate min_element/max_element instead of combined minmax_element
#define DEBUG_OUTPUT            // prints debug output
#define MAX_DEPTH 500           // sets maximum depth for kd-tree nodes

const int WIDTH = 1024, HEIGHT = 800; // screen dimensions

class Point;

enum Axis
{
    X,
    Y,
    Z
};
static const Axis Dimensions[] = { X, Y, Z };

static std::string AxisToString (const Axis axis)
{
    char c = 'X' + axis;
    std::string s{ c };
    return s;
}

// stores 3d points
class Point
{
private:
    float coords[3];

public:
    Point (float i_x, float i_y, float i_z) : coords{ i_x, i_y, i_z }
    {
    }

    float dim (Axis axis)
    {
        return coords[(int)axis];
    }

    std::string toString ()
    {
        return std::to_string(dim(X)) + ", " +
               std::to_string(dim(Y)) + ", " +
               std::to_string(dim(Z));
    }
};

// represent a node of the tree
struct Node
{
    Axis axis;
    Point* pt;
    Node* left;
    Node* right;

    Node (Axis axis, Point *pt, Node* left, Node* right)
        : axis(axis), pt(pt), left(left), right(right)
    {
    }

    ~Node ()
    {
        delete left;
        delete right;
    }

    std::string toString ()
    {
        return "splitting axis: " + AxisToString(axis) + ", point: " + (pt ? pt->toString() : "nullptr");
    }
};
#endif COMMON_H