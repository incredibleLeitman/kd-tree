#pragma once
#ifndef VIS_H
#define VIS_H

class KDTree;
struct GLFWwindow;
class Shader;

#include <iostream>

class Vis
{
private:
	KDTree &_tree;
	GLFWwindow* _window;
	Shader* _shader;

	const int WIDTH = 800, HEIGHT = 600;

	bool _showGrid; // toggle with key 'G' to show kd-tree cells

	int exitWithError (std::string code);
	int init ();
	int createWindow ();

	// opengl callbacks
	static void mouse_callback (GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback (GLFWwindow* window, double xoffset, double yoffset);
	void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods);

	void processInput();

public:
	Vis (KDTree &tree);
	void display();
};
#endif // VIS_H