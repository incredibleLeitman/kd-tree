#include "kdtree.h"

#include <algorithm>
#include <chrono>
#include <iostream>

auto axis_comparator (Axis axis)
{
    return [axis](Point* p1, Point* p2) {
        // TODO: use points as vectors or array or different comparators for each axis
        //return p1->pos[axis] < p2->pos[axis];
        if (axis == Axis::X)
        {
            return p1->x < p2->x;
        }
        else if (axis == Axis::Y)
        {
            return p1->y < p2->y;
        }
        else //if (axis == Axis::Z)
        {
            return p1->z < p2->z;
        }
    };
}

KDTree::KDTree (std::vector<Point*> points)
{
    std::cout << "building kd-tree with " << points.size() << " points... ";
    auto start = std::chrono::high_resolution_clock::now();
    _root = build(points);
    std::cout << " took " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
}

//KDTree::KDTree(Point* points, uint32_t size)
/*KDTree::KDTree(Point points[], uint32_t size)
{

}*/

Node * KDTree::build (std::vector<Point*> points)
{
    // TODO:
    // - check for max depth
    // - set maximum leaf node count
    if (points.empty()) { return nullptr; }

    // select splitting axis by choosing max extent
    Axis axis = Axis::X;
    float max_extent = 0.0f;
    //for (uint32_t idx_axis = 0; idx_axis < 3; ++idx_axis)
    static const Axis Dimensions[] = { X, Y, Z };
    for (Axis cand_axis : Dimensions)
    {
        // TODO: compare min / max seperate agains minmax
        #ifdef SEPERATE_MIN_MAX
            Point* min = *std::min_element(points.begin(), points.end(), axis_comparator(cand_axis));
            Point* max = *std::max_element(points.begin(), points.end(), axis_comparator(cand_axis));
            float extent = max->dim(cand_axis) - min->dim(cand_axis);
            #ifdef DEBUG_OUTPUT
                std::cout << "min: " << min->dim(cand_axis) <<
                    ", max: " << max->dim(cand_axis) <<
                    " -> extent: " << extent << " for axis " << axis << std::endl;
            #endif
        #else
            //const auto minmax = std::minmax_element(begin(points), end(points), axis_comparator(cand_axis));
            const auto minmax = std::minmax_element(points.begin(), points.end(), axis_comparator(cand_axis));
            float extent = (*minmax.second)->dim(cand_axis) - (*minmax.first)->dim(cand_axis);
            #ifdef DEBUG_OUTPUT
                std::cout << "min: " << (*minmax.first._Ptr)->dim(cand_axis) <<
                    ", max: " << (*minmax.second._Ptr)->dim(cand_axis) <<
                    " -> extent: " << extent << " for axis " << axis << std::endl;
            #endif
        #endif

        if (extent > max_extent)
        {
            max_extent = extent;
            axis = cand_axis;
        }
    }

    // choose the median as pivot and sort by splitting axis
    size_t pivot = points.size() / 2;
    std::nth_element(points.begin(), points.begin() + pivot, points.end(), axis_comparator(axis));

    std::vector<Point*> right(points.begin() + pivot + 1, points.end());
    //std::vector<Point*> left(points.begin(), points.begin() + pivot);
    points.resize(pivot);

    // TODO: add Node ctor
    Node* node = new Node();
    node->axis = axis;
    node->pt = (right.size() > 0) ? right[0] : nullptr;
    node->left = build(points);
    node->right = build(right);
    return node;
}