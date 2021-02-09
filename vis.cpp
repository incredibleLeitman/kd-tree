#include "vis.h"
#include "shader.h"
#include "camera.h"
#include "kdtree.h"

// #define GLFW_INCLUDE_NONE before including GLFW, or include glad bfore including glfw.
#include <GLFW/glfw3.h>

float lastX, lastY = 0;

Vis::Vis (KDTree &tree) : _tree(tree), _showGrid(false)
{
	init();
}

int Vis::init ()
{
	// initialize glfw library
	if (!glfwInit())
		return exitWithError("could not initialize glfw");

	// create openGL window
	if (createWindow() != 0)
		return exitWithError("could not create openGl window");

	// initialize glad with the pointer to the corresponding functions for the drivers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return exitWithError("Failed to initialize GLAD");
	}

    // create the viewport
    glViewport(0, 0, WIDTH, HEIGHT);

	// load and compile the vertex and fragment shaders
	_shader = new Shader("VertexShader.vert", "FragmentShader.frag");

	// init camera to be able to move around the scene
	_cam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	return 0;
}

int Vis::createWindow ()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set all the required options for GLFW
	_window = glfwCreateWindow(WIDTH, HEIGHT, "", NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		return exitWithError("could not initialize glfw window");
	}

	// Make the window's context current and set callbacks
	glfwMakeContextCurrent(_window);
	glfwSetWindowPos(_window, 250, 250);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // enables mouse capture
	// --------------------------------------------------------------------------------------------------------
	// problem:
	// https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback
	// You cannot use regular methods as callbacks, as GLFW is a C library and doesn’t know about objects and this pointers.
	// If you wish to receive callbacks to a C++ object, use static methods or regular functions as callbacks
	//
	// 1. use static callback
	//glfwSetFramebufferSizeCallback(_window, &Vis::framebuffer_size_callback);
	//	--> drawback: cannot access member variables
	// --------------------------------------------------------------------------------------------------------
	// 2. declare lambda function and bound to callback, providing the implementations:
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	});
	//	--> can be a lot of code thus not nice encapsulated
	// --------------------------------------------------------------------------------------------------------
	// 3. declare lamda function and delegate to implementation
	glfwSetWindowUserPointer(_window, this); // set window handle to this

	auto mouseCallback = [](GLFWwindow* window, double xpos, double ypos)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
	};
	glfwSetCursorPosCallback(_window, mouseCallback);

	//glfwSetScrollCallback(_window, &Vis::scroll_callback);

	auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
	};
	glfwSetKeyCallback(_window, keyCallback);

	return 0;
}

void Vis::display ()
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left
		 0.5f, -0.5f, 0.0f, // right
		 0.0f,  0.5f, 0.0f, // top

		 -0.5f, -5.5f, 0.0f, // left
		 0.5f, -5.5f, 0.0f, // right
		 0.0f, -4.5f, 0.0f  // top
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(_window))
	{
		// per-frame time logic
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();

		// render scene
		// ------------------------------------------------------------
		if (_showGrid)
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		else
			glClearColor(.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_shader->use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(_cam->Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		_shader->setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = _cam->GetViewMatrix();
		_shader->setMat4("view", view);

		_shader->setMat4("model", glm::mat4(1.0f));

		// render triangles
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	// de-allocate all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	//return EXIT_SUCCESS;
}

int Vis::exitWithError(std::string code)
{
	std::cout << "error: " << code << std::endl;
	return EXIT_FAILURE;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Vis::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static bool firstMouse = true;
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

	_cam->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Vis::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_cam->ProcessMouseScroll((float)yoffset);
}

// The callback function receives the keyboard key, platform-specific scancode, key action and modifier bits.
// ----------------------------------------------------------------------
void Vis::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
    {
		if (key == GLFW_KEY_G)
			_showGrid ^= true;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Vis::processInput ()
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);

	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		_cam->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		_cam->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		_cam->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		_cam->ProcessKeyboard(RIGHT, deltaTime);
}