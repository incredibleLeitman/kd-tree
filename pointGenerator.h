#pragma once
#ifndef POINT_GENERATOR_H
#define POINT_GENERATOR_H

#include <cstdlib>  // for srand, rand
#include <fstream> // ifstream
#include <ctime> // time
#include <string> // stoi

#include <OBJ_Loader.h> // used to load .obj files

class PointGenerator
{
public:
    static void init ()
    {
        static bool init = false;
        if (!init)
        {
            #ifdef _DEBUG
                srand(static_cast <unsigned> (0)); // fixed seed for testing
            #else
                srand(static_cast <unsigned> (time(0)));
            #endif
            init = true;
        }
    }

    static std::vector<float> read_file (const std::string& fileRnd)
    {
        objl::Loader loader;
        if (loader.LoadFile(fileRnd))
        {
            std::cout << "Vertices: " << loader.LoadedVertices.size() << std::endl;
        }
        else
        {
            std::cerr << "[X] Could not load file!" << std::endl;
            std::exit(1);
        }

        std::vector<float> vertices;
        for (const objl::Vertex& vertex : loader.LoadedVertices)
        {
            vertices.push_back(vertex.Position.X);
            vertices.push_back(vertex.Position.Y);
            vertices.push_back(vertex.Position.Z);
        }
        return vertices;
    }

    static std::vector<float> generate_3dpoint (const float min = 0, const float max = MAX_DIM)
    {
        init();
        return std::vector<float>
        {
            min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)),
            min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)),
            min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max))
        };
    }

    static std::vector<float> generate_triangles (const uint32_t count, const float min = 0, const float max = MAX_DIM)
    {
        init();

        std::vector<float> triangles(count*9);
        float EXT = 5;
        for (uint32_t i = 0; i < count; ++i)
        {
            // create the top triangle point for each dimension
            uint32_t start = i * 9;
            for (uint32_t dim = 0; dim < 3; ++dim)
            { 
                triangles[start + dim] = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
            }

            // than add the other left and right bottom corners by a fix extent
            uint32_t offset = start + 3;
            triangles[offset++] = triangles[start] - EXT/2;
            triangles[offset++] = triangles[start + 1] - EXT;
            triangles[offset++] = triangles[start + 2];

            triangles[offset++] = triangles[start] + EXT / 2;
            triangles[offset++] = triangles[start + 1] - EXT;
            triangles[offset++] = triangles[start + 2];
        }
        return triangles;
    }
};
#endif // POINT_GENERATOR_H