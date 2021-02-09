#pragma once
#ifndef VIS_H
#define VIS_H

class KDTree;
struct GLFWwindow;
class Shader;
class Camera;

#include <iostream>

class Vis
{
private:
	KDTree &_tree;
	GLFWwindow* _window;
	Shader* _shader;
	Camera* _cam;

	bool _showGrid; // toggle with key 'G' to show kd-tree cells

	// timing
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;

	// rendering
	float* _vertices;
	uint32_t _countVertices;
	unsigned int VBO, VAO;

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