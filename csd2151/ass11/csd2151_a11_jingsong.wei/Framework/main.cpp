/*!*****************************************************************************
\file main.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author YOUR NAME (DIGIPEN ID)
\par Course: CSD2151/CSD2150/CS250
\par Assignment:
\date 02/09/2024 (MM/DD/YYYY)
\brief This file has definitions of the main function
	   along with global variables and sub-functions used in main.
*******************************************************************************/
#include <iostream>

#include "common/debug.h"
#include "common/cg.h"

#include <GLFW/glfw3.h>

cg::Scene* pScene = nullptr;
glm::vec2 screen = { 0, 0 };
bool lbutton_down = false;
bool mode_shift = false;
/*
   This function serves as the callback parameter for
	  glfwSetKeyCallback function used in the main function

   Esc - close the app
*/
void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(pWindow, GL_TRUE);
		else if (key == 'W')
			pScene->camerasSetType(cg::CameraType::WALKING);
		else if (key == 'O')
			pScene->camerasSetType(cg::CameraType::ORBITING);
	}
	mode_shift = (mods == GLFW_MOD_SHIFT);
}

/*
   This function serves as the callback parameter for
	  glfwSetMouseButtonCallback function used in the main function
*/
void mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}
}

/*
   This function serves as the callback parameter for
	  glfwSetCursorPosCallback function used in the main function
*/
void cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos)
{
	if (!lbutton_down)
		return;
	if (mode_shift)
	{
		static double oldxpos = xpos;
		static double oldypos = ypos;
		pScene->lightsOnCursor(xpos - oldxpos, ypos - oldypos, &pScene->shader);
		oldxpos = xpos;
		oldypos = ypos;
	}
	else
	{
		static double oldxpos = xpos;
		static double oldypos = ypos;
		pScene->camerasOnCursor(xpos - oldxpos, ypos - oldypos, &pScene->shader);
		oldxpos = xpos;
		oldypos = ypos;
	}
}

/*
   This function serves as the callback parameter for
	  glfwSetScrollCallback function used in the main function
*/
void scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
{
	if (mode_shift)
		pScene->lightsOnScroll(xoffset, yoffset, &pScene->shader);
	else
		pScene->camerasOnScroll(xoffset, yoffset, &pScene->shader);
}

/*
   This function serves as the callback parameter for
	  glfwSetWindowSizeCallback function used in the main function
*/
void sizeCallback(GLFWwindow* pWindow, int width, int height)
{
	screen.x = static_cast<float>(width);
	screen.y = static_cast<float>(height);

	pScene->passes[0].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
	pScene->passes[1].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
}

/*
	The main function
*/
int main(int argc, char** argv)
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

#if defined( _DEBUG )
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* pWindow = glfwCreateWindow(WIDTH, HEIGHT, "OgresAssignment", NULL, NULL);
	if (!pWindow)
	{
		std::cerr << "Unable to create OpenGL context." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(pWindow);

	// Load the OpenGL functions.
	glewExperimental = GL_TRUE; // Needed for core profile
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;

		// Close pWindow and terminate GLFW
		glfwTerminate();

		// Exit program
		std::exit(EXIT_FAILURE);
	}

#if defined( _DEBUG )
	dumpGLInfo();
#endif

	std::cout << std::endl;
	std::cout << "The Ogres Assignment demo." << std::endl;
	std::cout << "Description:" << std::endl;
	std::cout << "   This app demonstrates a two-pass mesh rendering process for 231 objects" << std::endl;
	std::cout << "   using instancing, deferred shading technique and normal mapping." << std::endl;
	std::cout << "Interactions:" << std::endl;
	std::cout << "   - The camera movement is controlled by the mouse left button and wheel." << std::endl;
	std::cout << "   - Press W or O to switch between the camera movement types (orbiting or walking)." << std::endl;
	std::cout << "   - The light is controlled the same way when shift is pressed." << std::endl;
	std::cout << "   - The screen is resizable." << std::endl;
	std::cout << "   - Press the Esc key to close the app." << std::endl;
	std::cout << std::endl;

#if defined( _DEBUG )
	glDebugMessageCallback(debugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
		GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
#endif

	glfwSetKeyCallback(pWindow, keyCallback);
	glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(pWindow, cursorPosCallback);
	glfwSetScrollCallback(pWindow, scrollCallback);
	glfwSetWindowSizeCallback(pWindow, sizeCallback);

	try
	{
#define ANINSTANCE(x, z)\
         { MATERIAL_OBJECT, glm::translate(glm::mat4(1.0f), {x, 0.0f, z}) }

#define INSTANCES(x)\
                   ANINSTANCE(x, -20.0f-5.0f),\
                   ANINSTANCE(x, -18.0f-5.0f),\
                   ANINSTANCE(x, -16.0f-5.0f),\
                   ANINSTANCE(x, -14.0f-5.0f),\
                   ANINSTANCE(x, -12.0f-5.0f),\
                   ANINSTANCE(x, -10.0f-5.0f),\
                   ANINSTANCE(x,  -8.0f-5.0f),\
                   ANINSTANCE(x,  -6.0f-5.0f),\
                   ANINSTANCE(x,  -4.0f-5.0f),\
                   ANINSTANCE(x,  -2.0f-5.0f),\
                   ANINSTANCE(x,   0.0f-5.0f),\
                   ANINSTANCE(x,   2.0f-5.0f),\
                   ANINSTANCE(x,   4.0f-5.0f),\
                   ANINSTANCE(x,   6.0f-5.0f),\
                   ANINSTANCE(x,   8.0f-5.0f),\
                   ANINSTANCE(x,  10.0f-5.0f),\
                   ANINSTANCE(x,  12.0f-5.0f),\
                   ANINSTANCE(x,  14.0f-5.0f),\
                   ANINSTANCE(x,  16.0f-5.0f),\
                   ANINSTANCE(x,  18.0f-5.0f),\
                   ANINSTANCE(x,  20.0f-5.0f)

		cg::Scene scene =
		{
			// Vertex shader
			{
				#include "OgresAssignment.vert.glsl"
			},

			// Fragment shader
			{
				#include "OgresAssignment.frag.glsl"
			},

			// Passes
			{
				// Pass 0
				{
					// Rendering target
					{ cg::FBO::Type::Deferred, WIDTH, HEIGHT },

					// Viewport
					{ 0, 0, WIDTH, HEIGHT },

					// Background color
					BLACK,

					// Depth buffer
					ENABLE,

					// Objects
					{
						{   "bs_ears.obj",
							{
								INSTANCES(-10.0f),
								INSTANCES(-8.0f),
								INSTANCES(-6.0f),
								INSTANCES(-4.0f),
								INSTANCES(-2.0f),
								INSTANCES(0.0f),
								INSTANCES(2.0f),
								INSTANCES(4.0f),
								INSTANCES(6.0f),
								INSTANCES(8.0f),
								INSTANCES(10.0f),
							}
						}
					},

			// The camera
			{
				{ {0.0f, 4.0f, 5.0f} }
			},

			// Lights
			{
				DEFAULT
			},

			// Textures
			{
				{ { "ogre_diffuse.png" },   GL_TEXTURE3 }, // Channel 3
				{ { "ogre_normalmap.png" }, GL_TEXTURE4 }  // Channel 4                         
			},

			// Setup uniforms in the shader
			NULL
		},

			// Pass 1
			{
				// Rendering target
				DEFAULT,

				// Viewport
				{ 0, 0, WIDTH, HEIGHT },

				// Background color
				{ },

				// Depth buffer
				DISABLE,

				// Objects
				{
					{
						QUAD
					}
				},

			// The camera
			{

			},

			// Lights
			{

			},

			// Textures
			{

			},

			// Setup uniforms in the shader

		}
	}
		};

		pScene = &scene;

		while (!glfwWindowShouldClose(pWindow))
		{
			checkForOpenGLError(__FILE__, __LINE__);

			scene.render();

			glfwSwapBuffers(pWindow);
			glfwPollEvents();
		}

	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;

		// Close pWindow and terminate GLFW
		glfwTerminate();

		// Exit program
		std::exit(EXIT_FAILURE);
	}

#if defined( _DEBUG )
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
		GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

	// Close pWindow and terminate GLFW
	glfwTerminate();

	// Exit program
	return EXIT_SUCCESS;
}