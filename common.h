#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

// ----------------------------------------
// Considerations...
// ----------------------------------------
// - array vs vector
//      + could be faster --> test, needed?
// - check if chrono clock in finally block is slower than execution
// - maybe replace Axis enum with int -> easier iteration, stringification and extension
// - store multiple triangles for points, as common in standard meshes

// - fill kdtree with 3d points or triangles? -> implementation for both: center points and corners

// TODO:
// - outsource enums, structs and classes into own files
// - create struct/class for triangle
// - replace custom Point class with glm?
// - using references from lists --> better arrays
// - kd-tree using max iteration depth to abort?
// - compare with bruteforce
// - replace vis inorder tree iteration with recursive search --> also use for bruteforce
// - remove glm includes from common.h -> use implementation file

// - Ray::intersect -> only use triangle struct; return t instead of storing in ref param

#define SEPERATE_MIN_MAX        // use seperate min_element/max_element instead of combined minmax_element
//#define DEBUG_OUTPUT            // prints debug output
#define MAX_DEPTH 500           // sets maximum depth for kd-tree nodes
#define MAX_DIM 100             // maximum dimension for scene extent
#define SAVE_TRIANGLES        // kd-tree stores the center of each triangle as point
//#define SAVE_CORNERS            // kd-tree stores each corner point of each triangle
#define EPSILON 0.0000001       // minimum allowed difference for points to determine equality

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

    // TODO: make list of triangles?
    Triangle* triangle = nullptr;
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

    // Ray-triangle-intersection with the Möller–Trumbore algorithm
    /*bool intersects (Triangle& triangle, glm::vec3& hit, float t)
    {
        return intersects(*triangle.points[0], *triangle.points[1], *triangle.points[2], hit, t);
    }*/
    bool intersects (Triangle* triangle, glm::vec3& hit, float &t) const
    {
        return intersects(*triangle->points[0], *triangle->points[1], *triangle->points[2], hit, t);
    }

    bool intersects (Point &pt1, Point& pt2, Point& pt3, glm::vec3& hit, float &t) const
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

        if (abs(a) < EPSILON) return false; // This ray is parallel to this triangle.

        // it we are parallel to the triangle return false
        if (std::abs(a) < EPSILON)
        {
            return false;
        }

        float f = 1.0f / a;
        glm::vec3 s = origin - pt1;
        float u = f * glm::dot(s, h);

        if (u < 0.0 || u > 1.0) return false;

        glm::vec3 q = glm::cross(s, edge21);
        float v = f * glm::dot(dir, q);
        if (v < 0.0 || (u + v) > 1.0) return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        t = f * glm::dot(edge31, q);
        if (t > EPSILON)
        {
            hit = origin + dir * t;
            return true;
        }
        else
        {
            // This means that there is a line intersection but not a ray intersection.
            return false;
        }
    }
};
#endif // COMMON_H