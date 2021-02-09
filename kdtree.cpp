#include "kdtree.h"

#include <algorithm>
#include <chrono>
#include <iostream>

// comparator lambda for axis dependent comparison
static auto axis_comparator(Axis axis)
{
    return [axis](Point* p1, Point* p2)
    {
        return p1->dim(axis) < p2->dim(axis);
    };
}

KDTree::KDTree (std::vector<Point*> &points)
    :_depth(0)
{
    std::cout << "building kd-tree with " << points.size() << " points..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    _root = build(points, 0);
    std::cout << " took " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
}

KDTree::KDTree (float *vertices, uint32_t count)
    :_depth(0)
{
    std::cout << "building kd-tree with " << count << " points..." << std::endl;
    // - conversion from float[] to vector<Point*>
    std::vector<Point*> points(count/3);
    for (uint32_t idx = 0; idx < count; idx += 3)
    {
        points[idx/3] = new Point(vertices[idx], vertices[idx + 1], vertices[idx + 2]);
    }
    auto start = std::chrono::high_resolution_clock::now();
    _root = build(points, 0);
    // - build with arrays --> too much overhead
    //_root = build(vertices, count, 0);
    std::cout << " took " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
}

void KDTree::print ()
{
    std::cout << "kd tree with depth: " << _depth << std::endl;
    printNode(_root);
}

Node* KDTree::root ()
{
    return _root;
}

void KDTree::printNode (Node* node)
{
    if (node)
    {
        std::cout << node->toString() << std::endl;
        std::cout << "    l: " << (node->left ? node->left->toString() : "nullptr") << std::endl;
        std::cout << "    r: " << (node->right ? node->right->toString() : "nullptr") << std::endl;
        if (node->left) printNode(node->left);
        if (node->right) printNode(node->right);
    }
    else
    {
        std::cout << "nullptr" << std::endl;
    }
}

Node * KDTree::build (std::vector<Point*> &points, uint32_t depth)
{
    // TODO:
    // - set maximum leaf node count
    if (points.empty() || depth > MAX_DEPTH) { return nullptr; }

    if (depth > _depth) _depth = depth;

    #ifdef DEBUG_OUTPUT
        std::cout << "points: " << points.size() << std::endl;
        for (Point* pt : points)
        {
            std::cout << pt->toString() << std::endl;
        }
    #endif

    // select splitting axis by choosing max extent
    Axis axis = Axis::X;
    float max_extent = 0.0f;
    for (Axis cand : Dimensions)
    {
        // TODO: compare min / max seperate against minmax
        #ifdef SEPERATE_MIN_MAX
            Point* min = *std::min_element(points.begin(), points.end(), axis_comparator(cand));
            Point* max = *std::max_element(points.begin(), points.end(), axis_comparator(cand));
            float extent = max->dim(cand) - min->dim(cand);
            #ifdef DEBUG_OUTPUT
                std::cout << "min: " << min->dim(cand) <<
                    ", max: " << max->dim(cand) <<
                    " -> extent: " << extent << " for axis " << AxisToString(cand) << std::endl;
            #endif
        #else
            //const auto minmax = std::minmax_element(begin(points), end(points), axis_comparator(cand));
            const auto minmax = std::minmax_element(points.begin(), points.end(), axis_comparator(cand));
            float extent = (*minmax.second)->dim(cand) - (*minmax.first)->dim(cand);
            #ifdef DEBUG_OUTPUT
                std::cout << "min: " << (*minmax.first._Ptr)->dim(cand) <<
                    ", max: " << (*minmax.second._Ptr)->dim(cand) <<
                    " -> extent: " << extent << " for axis " << AxisToString(cand) << std::endl;
            #endif
        #endif

        if (extent > max_extent)
        {
            max_extent = extent;
            axis = cand;
        }
    }

    // choose the median as pivot and sort by splitting axis
    size_t pivot = points.size() / 2;
    std::nth_element(points.begin(), points.begin() + pivot, points.end(), axis_comparator(axis));

    Point* median = points[pivot];

    std::vector<Point*> right(points.begin() + pivot + 1, points.end());
    //std::vector<Point*> left(points.begin(), points.begin() + pivot);
    points.resize(pivot);

    return new Node(axis, median, build(points, depth + 1), build(right, depth + 1));
}