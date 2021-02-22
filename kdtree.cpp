#include "kdtree.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <queue>

// comparator lambda for axis dependent comparison
static auto axis_comparator(Axis axis)
{
    return [axis](Point* p1, Point* p2)
    {
        return p1->dim(axis) < p2->dim(axis);
    };
}

KDTree::KDTree (float *vertices, size_t count)
    :_depth(0)
{
    std::cout << "building kd-tree with " << count << " points..." << std::endl;
    // - variant 1: build with arrays --> too much overhead
    //_root = build(vertices, count, 0);

#if defined(SAVE_TRIANGLES)
    // conversion from float[] to vector<Point*>
    // add only center point of the triangle
    std::vector<Point*> points(count / 9);
    for (size_t idx = 0; idx < count; idx += 9)
    {
        float A[3] = { vertices[idx], vertices[idx + 1], vertices[idx + 2] };
        float B[3] = { vertices[idx + 3], vertices[idx + 4], vertices[idx + 5] };
        float C[3] = { vertices[idx + 6], vertices[idx + 7], vertices[idx + 8] };
        points[idx / 9] = new Point(
            (A[0] + B[0] + C[0]) / 3,
            (A[1] + B[1] + C[1]) / 3,
            (A[2] + B[2] + C[2]) / 3);
        points[idx / 9]->triangle = new Triangle(A, B, C);
    }
#elif defined(SAVE_CORNERS)
    // conversion from float[] to vector<Point*>
    // add every single point of the triangle

    // notes on  a few tests:
    // - not specifying size and just adding points doesn't decreases performance
    // - checking for existing points happens for standard meshes and also doesn't costs performance
    std::vector<Point*> points;
    Point *A = nullptr;
    Point* B = nullptr;
    Point* C = nullptr;
    for (uint32_t idx = 0; idx < count; idx += 3)
    {
        size_t idx_pt = idx/3;
        Point *pt = new Point(vertices[idx], vertices[idx + 1], vertices[idx + 2]);
        //std::vector<Point*>::iterator it = std::count(points.begin(), points.end(), pt);
        //std::vector<Point*>::iterator it = std::find(points.begin(), points.end(), pt);
        std::vector<Point*>::iterator it = std::find_if(std::begin(points), std::end(points), [pt](Point* cand) {return (*cand == *pt); });
        if (it == points.end()) points.push_back(pt);
        else pt = *it;

        uint8_t mod = (idx_pt + 1) % 3;
        if (mod == 1)       A = pt;
        else if (mod == 2)  B = pt;
        else if (mod == 0)
        {
            C = pt;

            // create the resulting triangle afterwards and set reference to all points
            Triangle* triangle = new Triangle(A, B, C);
            A->triangles.push_back(triangle);
            B->triangles.push_back(triangle);
            C->triangles.push_back(triangle);
        }
    }
#endif

    auto start = std::chrono::high_resolution_clock::now();
    _root = build(points, 0);
    std::cout << "\ttook\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
    std::cout << "max depth: " << _depth << " for total nodes: " << _nodes << std::endl;
}

void KDTree::print ()
{
    std::cout << "kd tree with depth: " << _depth << std::endl;
    if (_root) printNode(*_root);
}

Node* KDTree::root ()
{
    return _root;
}

void KDTree::printNode (Node& node)
{
    std::cout << node.toString() << std::endl;
    std::cout << "    l: " << (node.left ? node.left->toString() : "nullptr") << std::endl;
    std::cout << "    r: " << (node.right ? node.right->toString() : "nullptr") << std::endl;
    if (node.left) printNode(*node.left);
    if (node.right) printNode(*node.right);
}

Node* KDTree::build (std::vector<Point*> &points, uint32_t depth)
{
    size_t count = points.size();
    if (count == 0 /*points.empty()*/)
    { 
        return nullptr;
    }
    else if (depth > MAX_DEPTH)
    {
        // TODO: store remaining points as leaf node?
        std::cerr << "tree depth exeded maximum " << MAX_DEPTH << ", abort!" << std::endl;
        return nullptr;
    }
    // TODO: set maximum leaf node count
    /*else if (count == 1)
    {
        return nullptr;

        // adding no spliting axis or children, just adding the point
        //float extension[3];
        //return new Node(Axis::X, extension, points[0], nullptr, nullptr);
    }*/

    if (depth > _depth) _depth = depth;

    /*#ifdef DEBUG_OUTPUT
        std::cout << "points: " << points.size() << std::endl;
        for (Point* pt : points)
        {
            std::cout << pt->toString() << std::endl;
        }
    #endif*/

    // select splitting axis by choosing max extent
    Axis axis = Axis::X;
    float extension[3];
    float max_extent = 0.0f;
    for (Axis cand : Dimensions)
    {
        // compare min / max seperate against minmax
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

        extension[(int)cand] = (extent == 0) ? MAX_DIM : extent;
        #ifdef USE_FIRST_AXIS   // only updates axis if extent is larger -> otherwise let the first choosen
            if (extent > max_extent)
        #else                   // updates splitting plane if it has the same extent as the previous choosen
            if (extent >= max_extent)
        #endif
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
    points.resize(pivot);

    // update extension for resulting axis
    extension[(int)axis] = 0;

    #ifdef DEBUG_OUTPUT
        std::cout << " -> choosing axis " << AxisToString(axis) << " with max extent: " << max_extent << std::endl;
    #endif

    _nodes++;
    return new Node(axis, extension, median, build(points, depth + 1), build(right, depth + 1));
}

const Triangle* KDTree::raycast (const Ray ray)
{
    std::cout << "testing intersection with ray " << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    float nearest = MAX_DIM;
    const Triangle* nearest_triangle = nullptr;
    glm::vec3 result;
    _iterated = 0;
    raycastNode(_root, nearest_triangle, result, ray, nearest);

    std::cout << "\ttook\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds iterating " << _iterated << " of total " << _nodes  << " nodes" << std::endl;
    return nearest_triangle;
}

void KDTree::raycastNode (Node* node, const Triangle*& nearest_triangle, glm::vec3& result, const Ray& ray, float& nearest)
{
    if (node == nullptr) return;

    _iterated++;

    // if a collision it detected, that is closer to the ray origin than the closest previous collision, store it
    glm::vec3 current_result(0, 0, 0);
    float current_distance;

#ifdef SAVE_CORNERS
    for (const Triangle* triangle : node->pt->triangles)
    {
        if (ray.intersects(triangle, current_result, current_distance))
#else
    if (ray.intersects(node->pt->triangle, current_result, current_distance))
#endif
    {
        if (current_distance < nearest)
        {
            nearest = current_distance;
            #ifdef SAVE_CORNERS
                nearest_triangle = triangle;
            #else
                nearest_triangle = node->pt->triangle;
            #endif
            result = current_result;
        }
    }

#ifdef SAVE_CORNERS
    } // for each triangle
#endif

    // Is the ray origin within the left or right child node bounding box?
    Node* near = ray.origin[node->axis] > node->pt->dim(node->axis) ? node->right : node->left;
    if (ray.dir[node->axis] == 0.0)
    {
        // The ray is parallel to the splitting axis, so we only need to check within this box.
        raycastNode(near, nearest_triangle, result, ray, nearest);
    }
    else
    {
        // Calculate the distance from the ray origin to the splitting axis
        float t = (node->pt->dim(node->axis) - ray.origin[node->axis]) / ray.dir[node->axis];

        // Check this side for intersections up to the distance of the currently best
        // intersection
        raycastNode(near, nearest_triangle, result, ray, nearest);

        // If the far side is closer than the distance to the best current intersection, check that side too
        if (t < nearest)
        {
            Node* far = (near == node->right) ? node->left : node->right;
            raycastNode(far, nearest_triangle, result, ray, nearest);
        }
    }
}

const Triangle* KDTree::bruteforce (Ray ray) const
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "brute force" << std::endl;

    // breadth first non-recursive traversion of kd-tree
    std::queue<Node*> q;
    const Triangle* nearest_triangle = nullptr;
    Node* cur = _root;
    q.push(cur);
    uint32_t nodes = 0;
    glm::vec3 current_result(0, 0, 0);
    float current_distance;
    float nearest = MAX_DIM;
    while (!q.empty())
    {
        nodes++;
        cur = q.front();
        q.pop();

    #ifdef SAVE_CORNERS
        for (const Triangle* triangle : cur->pt->triangles)
        {
            if (ray.intersects(triangle, current_result, current_distance))
    #else
        if (ray.intersects(cur->pt->triangle, current_result, current_distance))
    #endif
        {
            if (current_distance < nearest)
            {
                nearest = current_distance;
                #ifdef SAVE_CORNERS
                    nearest_triangle = triangle;
                #else
                    nearest_triangle = cur->pt->triangle;
                #endif
            }
            //break; // can not break because another node may be closer!
        }

    #ifdef SAVE_CORNERS
        } // for each triangle
    #endif

        if (cur->left) q.push(cur->left);
        if (cur->right) q.push(cur->right);
    }
    std::cout << "\tneeded\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
    return nearest_triangle;
}