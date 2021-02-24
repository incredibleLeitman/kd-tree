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
- -p                    starts in performance mode (just cmd), overrides -v flag
- -f <file>             loads vertices from given file (.obj file format)
- --rngcount <count>    generates the given amount of random triangles

## vis controls

(only available for windows builds. May work if a desktop manager is available, but to build the include and lib folders have to be provided in the makefile)

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

all timing data were recorded in x64 release mode using Microsoft Visual C++ and Gnu compiler (with -O3 flag) and averaged for at least 10 iterations, specification in microseconds. (Processor: Intel Core i7-6700K @4GHz)

Additionally to random generated triangles, the following mesh files were used:
- sphere (Vertices: 960)
- monkey suzanna (Vertices: 1968)
- noobPot (Vertices: 18960)

### building the kd-tree

#### std::minmax_element vs SEPERATE_MIN_MAX

std provides a combined minmax_element which calculates min and max together instead of seperate calls to min_element and max_element, which could be used to get a performance increase for building the tree (as those are calculated for every node). The results however show, that there is almost no difference, using gcc seperate implementation even is slightly faster.


| triangles        | seperate (msvc)       | combined (msvc)          | seperate (gcc)       | combined (gcc)          |
| ------------- |:-------------:|:-----:|:-------------:| -----:|
|    10.000      |    6546 |    4771 | 3147 | 3585 |
|   100.000      |   77366 |   69256 | 48785 | 53292 |
| 1.000.000      | 1319135 | 1035246 | 1022325 | 896311 |
| sphere         |     122 |     140 | 62 | 77 |
| monkey         |     264 |     373 | 139 | 164 |
| noobPot        |    3094 |    2468 | 1691 | 1991 |

#### store corners vs center of triangles

Although storing center points of all triangles need less memory than storing all corner points and works well for random generated triangles when there is no overlapping, the mode for storing corners is more suitable for "real" meshes, where most points are connected to multiple triangles.

| triangles        | corners (msvc)       | center (msvc)          | corners (gcc)       | center (gcc)          |
| ------------- |:-------------:|:-----:|:-------------:| -----:|
|    10.000      | 19490 | 5536 | 3102 | 2923 |
|   100.000      | 271903 | 76847 | 48210 | 48391 |
| 1.000.000      | 4031645 | 1258584 | 977806 | 988066 |
| sphere         | 348 | 129 | 73 | 70 |
| monkey         | 668 | 271 | 153 | 138 |
| noobPot        | 5064 | 2958 | 1677 | 1719 |

### raytracing

The big advantage of a kd-tree is, that compared to a simple bruteforce variant, the stored data is spatially ordered, thus not all nodes have to be iterated to find the nearest intersection, which leads to much faster timings.

Because in corner storing mode multiple triangles are tested for each point, the performance can be increased if a cache is setup.
As shown, this has a minor performance gain for the tested mesh files, but almost no impact for random generated triangles, as there are no shared triangles between all points.

- x...  mode with stored triangle center
- o...  mode with stored corner points
- +c... cache enabled


msvc++

| triangles        | raycast x       | bruteforce x          | raycast o       | bruteforce o       | raycast o+c          | bruteforce o+c           |
| ------------- |:-------------:|:-----:|:-----:|:-----:|:-------------:| -----:|
|    10.000      | 39 | 1053 | 647 | 692 | 132 | 4184 |
|   100.000      | 74 | 20545 | 3036| 3290 | 326 | 56003 |
| 1.000.000      | 709 | 213203| 13526| 24444 | 3083 | 661706 |
| sphere         | 31 | 33 | 27 | 21 | 28 | 72 |
| monkey         | 28 | 52 | 37 | 31 | 63 | 153 |
| noobPot        | 243 | 613 | 334 | 230 | 270 | 1064|

gcc
%TODO%


### misc (TODO)

#### QUEUE vs STACK

comparing non-recursive tree iteration using inorder depth first over breadth first search

#### chrono in finally block

checking if using chrono within a try or finally block makes a difference


## references

- Dissertation thesis Heuristic Ray Shooting Algorithms - Vlastimil Havran
- Fast kd-Tree Construction for 3D-rendering Algorithms like Ray Tracing - Sajid Hussain and Håkan Grahn 
- Using k-d Trees for Robust 3D Point Pattern Matching - Baihua Li Horst Holstein
- K-D Tree: build and search for the nearest neighbor - https://www.youtube.com/watch?v=ivdmGcZo6U8
- ALGO slides by Alexander Nimmervoll (especially alg_v7 k-d Tree Raycasting)
