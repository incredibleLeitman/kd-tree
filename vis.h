#pragma once
#ifndef VIS_H
#define VIS_H

class KDTree;
struct Node;
struct Triangle;
struct GLFWwindow;
class Shader;
class Camera;

#include <iostream>

class Vis
{
private:
	KDTree &_tree;
	const Triangle* _triangle;	// selected triangle through raycast -> nullptr if nothing hit
	GLFWwindow* _window;
	Shader* _shader;
	Camera* _cam;

	bool _showGrid;				// toggle with key 'G' to show kd-tree cells
	bool _showLines = true;		// toogle between polygon fill and wireframe lines using key 'P'
	bool _showRayCast;			// toogle raycast display with 'R'

	float _lastX, _lastY = 0;	// last stored mouse location

	size_t _step = 0;

	// timing
	float deltaTime = 0.0f;		// time between current frame and last frame
	float lastFrame = 0.0f;

	// rendering
	float* _vertices;
	size_t _countVertices;
	unsigned int VBO, VAO;				// vertex array and buffer for triangles
	unsigned int VAOPlane, VBOPlane;	// vao and vbo for splitting planes
	unsigned int VAOLine, VBOLine;		// vao and vbo for raycast

	int exitWithError (std::string code);
	int init ();
	int createWindow ();

	// opengl callbacks
	void mouse_callback (GLFWwindow* window, double xpos, double ypos);
	void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
	void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods);
	void processInput();

	void draw_splitting_plane(Node* cur, size_t colormod);

public:
	Vis (KDTree &tree, float *vertices, size_t countVertices);
	void display();
};
#endif // VIS_H