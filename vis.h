#pragma once
#ifndef VIS_H
#define VIS_H

class KDTree;
struct Triangle;
struct GLFWwindow;
class Shader;
class Camera;

#include <iostream>

class Vis
{
private:
	KDTree &_tree;
	const Triangle* _triangle; // selected triangle through raycast -> nullptr if nothing hit
	GLFWwindow* _window;
	Shader* _shader;
	Camera* _cam;

	bool _showGrid; // toggle with key 'G' to show kd-tree cells
	bool _showLines; // toogle between polygon fill and lines

	// timing
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;

	// rendering
	float* _vertices;
	uint32_t _countVertices;
	unsigned int VBO, VAO;				// vertex array and buffer for triangles
	unsigned int VAOPlane, VBOPlane;	// vao and vbo for splitting planes

	int exitWithError (std::string code);
	int init ();
	int createWindow ();

	// opengl callbacks
	void mouse_callback (GLFWwindow* window, double xpos, double ypos);
	void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
	void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods);

	void processInput();

public:
	Vis (KDTree &tree, float *vertices, uint32_t countVertices);
	void display();
};
#endif // VIS_H