#pragma once
#ifndef POINT_GENERATOR_H
#define POINT_GENERATOR_H

#include <list>
#include <cstdlib>  // for srand, rand
#include <fstream> // ifstream
#include <ctime> // time
#include <string> // stoi
//#include <iostream> // cin, cout
//#include <algorithm>

// TODO:
// finish 3d point generation from file (https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c)
// really generate / read triangles instead of points
// preallocating vector / array and use emplace_back

class PointGenerator
{
public:
    static std::vector<Point*> read_file (const std::string& fileRnd)
    {
        std::vector<Point*> points;
        std::ifstream in(fileRnd.c_str());
        if (in.is_open())
        {
            std::string line;
            std::getline(in, line); // get first line for explizit value count
            int count = std::stoi(line);
            float x = 0, y = 0, z = 0;
            for (int i = 0; i < count; ++i)
            {
                getline(in, line);
                //getline(in, line, ','); // could also be used slightly different but includes newlines :/
                size_t pos = line.find(',');
                if (pos != std::string::npos)
                {
                    x = std::stof(line.substr(0, pos));
                    y = std::stof(line.substr(pos + 1));
                    z = std::stof(line.substr(pos + 1));
                    points.push_back(new Point(x, y, z));
                }
                else
                {
                    throw "invalid format!";
                }
            }
        }
        return points;
    }

    static std::vector<Point*> generate_points (const int count, const float min = 0.0, const float max = MAX_DIM)
    {
        std::vector<Point*> points;

#ifdef _DEBUG
        srand(static_cast <unsigned> (0)); // fixed seed for testing
#else
        srand(static_cast <unsigned> (time(0)));
#endif

        float x, y, z;
        for (int i = 0; i < count; ++i)
        {
            x = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
            y = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
            z = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
            points.push_back(new Point(x, y, z));
        }

        return points;
    }
};
#endif // POINT_GENERATOR_H