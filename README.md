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

## references
- Dissertation thesis Heuristic Ray Shooting Algorithms - Vlastimil Havran
- Fast kd-Tree Construction for 3D-rendering Algorithms like Ray Tracing - Sajid Hussain and Håkan Grahn 
- Using k-d Trees for Robust 3D Point Pattern Matching - Baihua Li Horst Holstein
- K-D Tree: build and search for the nearest neighbor - https://www.youtube.com/watch?v=ivdmGcZo6U8
- ALGO slides by Alexander Nimmervoll (especially alg_v7 k-d Tree Raycasting)
