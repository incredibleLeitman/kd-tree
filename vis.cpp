#include "vis.h"
#include "shader.h"
#include "camera.h"
#include "kdtree.h"

// #define GLFW_INCLUDE_NONE before including GLFW, or include glad bfore including glfw.
#include <GLFW/glfw3.h>
#include <stack>

float lastX, lastY = 0;

Vis::Vis (KDTree &tree, float* vertices, uint32_t countVertices)
	: _tree(tree), _vertices(vertices), _countVertices(countVertices), _showGrid(false)
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

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _countVertices, _vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind the VAO so other VAO calls won't accidentally modify this VAO
	glBindVertexArray(0);

	// uncomment this call to draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xpos, double ypos)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
	});

	glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
	});

	glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		static_cast<Vis*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
	});

	return 0;
}

void Vis::display ()
{
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
		glClearColor(.0f, .0f, .0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_shader->use();
		_shader->setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(_cam->Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		_shader->setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = _cam->GetViewMatrix();
		_shader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		_shader->setMat4("model", model);

		// render triangles
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, _countVertices/3); // sides * triangles * vertices, e.g 36 for 18 values

		// we configurate the cube VAO for the boxes
		/*
		static constexpr float boxpoints[48] = {
		0,0,0,
		0,0,-1,
		-1,0,-1,
		-1,0,0,
		0,0,0,
		0,-1,0,
		0,-1,-1,
		0,0,-1,
		-1,0,-1,
		-1,-1,-1,
		0,-1,-1,
		0,-1,0,
		-1,-1,0,
		-1,0,0,
		-1,-1,0,
		-1,-1,-1
		};
		*/

		static constexpr float planepoints[16] = {
			0,0,0,
			0,1,0,
			1,1,0,
			1,0,0
		};

		unsigned int VAOCube, VBOCube;
		glGenVertexArrays(1, &VAOCube);
		glGenBuffers(1, &VBOCube);
		glBindVertexArray(VAOCube);
		glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planepoints), planepoints, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		
		if (_showGrid)
		{
			std::cout << "----------------------------------------------------" << std::endl;
			_shader->setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));

			glBindVertexArray(VAOCube);

			/*glBindVertexArray(pointVAO);
			pointShader.use();
			pointShader.setMat4("projection", projection);
			pointShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			glBindVertexArray(wireCubeVAO);
			for (auto box : tree.boxes) {
				model = glm::mat4(1.0f);
				box.getTransformMatrix(model);
				pointShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
				pointShader.setMat4("model", model);
				glDrawArrays(GL_LINE_STRIP, 0, 16);
			}*/
			
			// temp inorder non-recursive traversion of kd-tree
			std::stack<Node*> s;
			Node* cur = _tree.root();
			while (cur != NULL || s.empty() == false)
			{
				while (cur != NULL)
				{
					s.push(cur);
					cur = cur->left;
				}

				cur = s.top();
				s.pop();

				std::cout << cur->toString() << std::endl;

				model = glm::mat4(1.0f);
				// get plane transform and rotation
				//model = glm::translate(model, glm::vec3(xMax, yMax, zMax));
				model = glm::translate(model, cur->pt->toVec3());
				//model = glm::scale(model, glm::vec3(xMax - xMin, yMax - yMin, zMax - zMin));
				model = glm::scale(model, cur->extent->toVec3());
				model = glm::rotate(model, glm::radians(90.0f), AxisToVec3(cur->axis));

				_shader->setMat4("model", model);
				//glDrawArrays(GL_LINE_STRIP, 0, 16);
				//glDrawArrays(GL_TRIANGLES, 0, 36);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);

				cur = cur->right;
			}
		}

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