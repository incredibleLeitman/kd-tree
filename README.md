# kd-tree
Assignment for ALGO (algorithms and datastructures) @FHTechnikumWien

![Screenshot](teapot.png?raw=true "noobpot")

## dependencies

- glfw3 https://github.com/glfw/glfw
- glad https://glad.dav1d.de/
- khr https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h
- glm https://github.com/g-truc/glm
- OBJ_Loader.h https://github.com/Bly7/OBJ-Loader or my updated fork https://github.com/incredibleLeitman/OBJ-Loader

place header file in the "include" folder, libraries in "lib", the file "glad.c" have to be in included in the project and compiled.

## commandline arguments

- -v                    starts with visualization if provided (else just commandline)
- -f <file>             loads vertices from given file (.obj file format)
- --rngcount <count>    generates the given amount of random triangles

## vis controls

- move the observer camera with WASD keys
- space bar to shoot a raycast in the current view direction
- key 'P' toggles wireframe display
- key 'G' toggles grid (splitting planes) display
- numpad keys '+' and '-' in/decreases the shown splitting planes
- key 'R' toggles between the raycast drawing line
- escape to close the window and close app

## performance mode controls

- 'x' to close
- 'r' for raycast from center (0, 0, 0) to random point
- 'c' for raycast between to random points

## timings

all timing data were recorded in x64 release mode using Microsoft Visual C++ and Gnu compiler and averaged for at least 10 iterations, specification in microseconds.

Additionally to random generated triangles, the following mesh files were used:
- sphere (Vertices: 960)
- monkey suzanna (Vertices: 1968)
- noobPot (Vertices: 18960)

### building the kd-tree

std provides a combined minmax_element which calculates min and max together instead of seperate calls to min_element and max_element, which could be used to get a performance increase for building the tree (as those are calculated for every node). The results however show, that there is almost no difference, using gcc seperate implementation even is slightly faster.


| triangles        | seperate (msvc)       | combined (msvc)          | seperate (msvc)       | combined (msvc)          |
| ------------- |:-------------:|:-----:|:-------------:| -----:|
|    10.000      |    6546 |    4771 | | |
|   100.000      |   77366 |   69256 | | |
| 1.000.000      | 1319135 | 1035246 | | |
| sphere         |     122 |     140 | | |
| monkey         |     264 |     373 | | |
| noobPot        |    3094 |    2468 | | |

### raytracing






## references

- Dissertation thesis Heuristic Ray Shooting Algorithms - Vlastimil Havran
- Fast kd-Tree Construction for 3D-rendering Algorithms like Ray Tracing - Sajid Hussain and Håkan Grahn 
- Using k-d Trees for Robust 3D Point Pattern Matching - Baihua Li Horst Holstein
- K-D Tree: build and search for the nearest neighbor - https://www.youtube.com/watch?v=ivdmGcZo6U8
- ALGO slides by Alexander Nimmervoll (especially alg_v7 k-d Tree Raycasting)
