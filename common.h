#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

// --------------------------------------------------------------------------------
// Considerations...
// --------------------------------------------------------------------------------
// - array vs vector
//      + could be faster --> test, needed?
// - check if chrono clock in finally block is slower than in execution
// - maybe replace Axis enum with int -> easier iteration, stringification and extension
// - fill kdtree with center points or corners for triangles?
//      -> implementation for both
//      -> corners could be improved to reduce vertex count (store multiple triangles, common in standard meshes):
//          if more vertices share a triangle, make a list for each
//          --> render routine musst be updated to triangle fan

// TODO:
// --------------------------------------------------------------------------------
// - show raycast hit as point or omit the param
// - add bounding boxes display to vis in addition to splitting planes
// - set maximum leaf node count (also if max_depth is exceeded)
// - for corner storage to be efficient need to use a map instead of vector -> search O(1) instead of O(n)
// - performance mode: parse vector input for custom point

// REFACTOR:
// --------------------------------------------------------------------------------
// - Ray::intersect -> only use triangle struct; return t instead of storing in ref param
// - outsource enums, structs and classes into own files
// - replace custom Point class with glm::vec3?
// - replace macros with cmd args
// - remove glm includes from common.h -> use implementation file
// --------------------------------------------------------------------------------

#define SEPERATE_MIN_MAX        // use seperate min_element/max_element instead of combined minmax_element
//#define DEBUG_OUTPUT          // prints debug output
#define MAX_DEPTH 500           // sets maximum depth for kd-tree nodes
#define MAX_DIM 100             // maximum dimension for scene extent
#define SAVE_TRIANGLES        // kd-tree stores the center of each triangle as point
//#define SAVE_CORNERS            // kd-tree stores each corner point of each triangle
#define EPSILON 0.0000001       // minimum allowed difference for points to determine equality
#define USE_FIRST_AXIS          // if set, takes the first splitting axis more have the same extent
//#define USE_CACHE               // marks triangles as checked (for corner store mode)

// defines wheter use a stack inorder depth first search oder a queue for breadth first search
#define QUEUE
//#define STACK

// defines triangle axis placement for test mode
//#define AXIS_X
#define AXIS_Y_Z
//#define AXIS_X_Y_Z

const int WIDTH = 1024, HEIGHT = 800; // screen dimensions

class Point;
struct Triangle;

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
    Point (float coords[3]) : coords{ coords[0], coords[1], coords[2] }
    {
    }

    Point (float i_x, float i_y, float i_z) : coords{ i_x, i_y, i_z }
    {
    }

    // overload access operator to allow provide axis
    float operator[](Axis i) const { return coords[(int)i]; }
    const float& operator[](Axis i) { return coords[(int)i]; }

    // operator for value comparison
    bool operator != (const Point& rhs) { return coords[0] != rhs.coords[0] || coords[1] != rhs.coords[1] || coords[2] != rhs.coords[2]; }
    bool operator != (const Point& rhs) const { return coords[0] != rhs.coords[0] || coords[1] != rhs.coords[1] || coords[2] != rhs.coords[2]; }

    float dim (Axis axis) const
    {
        return coords[(int)axis];
    }

    std::string toString () const
    {
        return std::to_string(dim(X)) + ", " +
               std::to_string(dim(Y)) + ", " +
               std::to_string(dim(Z));
    }

    glm::vec3 toVec3 () const
    {
        return glm::vec3(coords[0], coords[1], coords[2]);
    }

    #ifdef SAVE_CORNERS
        std::vector<Triangle*> triangles;
    #else
        Triangle* triangle = nullptr;
    #endif
};

// collection for point triple to represent a triangle
struct Triangle
{
    Point* points[3];

    Triangle (Point* pt1, Point* pt2, Point* pt3) : points{ pt1, pt2, pt3 }
    {
    }

    Triangle (float pt1[3], float pt2[3], float pt3[3]) : points{ new Point(pt1), new Point(pt2), new Point(pt3) }
    {
    }

    std::string toString () const
    {
        return points[0]->toString() + ", " + points[1]->toString() + ", " + points[2]->toString();
    }

#if defined(SAVE_CORNERS) && defined(USE_CACHE)
    uint8_t idxRay = 0; // used to cache triangle check for raycast id
#endif
};

// represent a node of the tree
struct Node
{
    Axis axis;
    Point* extent;
    Point* pt;
    Node* left;
    Node* right;

    Node (Axis axis, float extent[3], Point *pt, Node* left, Node* right)
        : axis(axis), extent(new Point(extent)), pt(pt), left(left), right(right)
    {
    }

    ~Node ()
    {
        delete left;
        delete right;
    }

    std::string toString () const
    {
        return "splitting axis: " + AxisToString(axis) + ", point: " + (pt ? pt->toString() : "nullptr") + ", extension: " + extent->toString();
    }
};

// ray from origin point to destination; length is infinite
struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir;

    Ray (glm::vec3 dir, glm::vec3 origin = glm::vec3(0, 0, 0))
        : origin(origin), dir(dir)
    {
    }

    Ray (float dirX, float dirY, float dirZ, float origX = 0, float origY = 0, float origZ = 0)
        : origin(glm::vec3(origX, origY, origZ)), dir(glm::vec3(dirX, dirY, dirZ))
    {
    }

    // Ray-triangle-intersection with the Möller–Trumbore algorithm
    //bool intersects (Triangle& triangle, glm::vec3& hit, float t)
    bool intersects (const Triangle* triangle, glm::vec3& hit, float &t) const
    {
        return intersects(*triangle->points[0], *triangle->points[1], *triangle->points[2], hit, t);
    }

    bool intersects (const Point &pt1, const Point& pt2, const Point& pt3, glm::vec3& hit, float &t) const
    {
        return intersects(pt1.toVec3(), pt2.toVec3(), pt3.toVec3(), hit, t);
    }

    bool intersects (const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3, glm::vec3& hit, float &t) const
    {
        glm::vec3 edge21 = pt2 - pt1;
        glm::vec3 edge31 = pt3 - pt1;
        //glm::vec3 normal = glm::normalize(glm::cross(edge21, edge31));
        //float h = glm::dot(-normal, pt1);
        //float a = glm::dot(normal, dir);

        glm::vec3 h = glm::cross(dir, edge31);
        float a = glm::dot(edge21, h);

        // parallel to the triangle so return false
        if (abs(a) < EPSILON) return false;

        float f = 1.0f / a;
        glm::vec3 s = origin - pt1;
        float u = f * glm::dot(s, h);

        if (u < 0.0 || u > 1.0) return false;

        glm::vec3 q = glm::cross(s, edge21);
        float v = f * glm::dot(dir, q);
        if (v < 0.0 || (u + v) > 1.0) return false;

        // compute t to find out where the intersection point is on the line
        t = f * glm::dot(edge31, q);
        if (t > EPSILON)
        {
            hit = origin + dir * t;
            return true;
        }
        else // there is a line intersection but not a ray intersection.
        {
            return false;
        }
    }
};
#endif // COMMON_H