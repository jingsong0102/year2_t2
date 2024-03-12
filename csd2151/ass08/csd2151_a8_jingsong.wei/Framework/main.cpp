/*!*****************************************************************************
\file main.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 8 (NightSky App)
\date 03/06/2024 (MM/DD/YYYY)
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
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(pWindow, GL_TRUE);
	if (action == GLFW_PRESS)
		if (key == 'W')
			pScene->camerasSetType(cg::CameraType::WALKING);
		else if (key == 'O')
			pScene->camerasSetType(cg::CameraType::ORBITING);
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

	GLFWwindow* pWindow = glfwCreateWindow(WIDTH, HEIGHT, "NightSkyAssignment - PBR", NULL, NULL);
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
	std::cout << "The NightSkyAssignment demo." << std::endl;
	std::cout << "Description:" << std::endl;
	std::cout << "   This app demonstrates a multiple-pass mesh rendering process with vertex and" << std::endl;
	std::cout << "   fragment shaders that implement different types of shadings: PBR, fog, cartoon, and discard." << std::endl;
	std::cout << "Interactions:" << std::endl;
	std::cout << "   - Press W or O to switch between the camera movement types (orbiting or walking)." << std::endl;
	std::cout << "   - The camera movement is controlled by the mouse left button and wheel." << std::endl;
	std::cout << "   - The light is controlled the same way when shift is pressed." << std::endl;
	std::cout << "   - The screen is resizable." << std::endl;
	std::cout << "   - You can press key Esc to close the app." << std::endl;
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
		cg::Scene::Texture texture = {
		  { "night_negx.png", "night_posx.png",
			"night_posy.png", "night_negy.png",
			"night_posz.png", "night_negz.png"  }
		};

		cg::Camera camera = { { 0.0f, 1.0f, 5.0f }, 75.0f };

		cg::Scene scene =
		{
			// Vertex shader
			{
				#include "NightSkyAssignment.vert.glsl"
			},

			// Fragment shader
			{
				#include "NightSkyAssignment.frag.glsl"
			},

			// Passes
			{
				// Pass 0 (render the skybox)
				{
					// Rendering target
					DEFAULT,

					// Viewport
					{ 0, 0, WIDTH, HEIGHT },

					// Color to clear buffers
					{ },

					// Depth test
					ENABLE,

					// Objects
					{
						{
						   SKYBOX
						}
					},

			// The camera
			{
				{ cg::CameraType::ORBITING, { 0.0f, 2.0f, 10.0f }, { 0.0f, 1.0f, 0.0f }, 75.0f }
			},

			// Lights
			{

			},

			// Textures
			{
				texture
			},

			// Setup uniforms in the shader
			NULL
		},

			// Pass 1 (render the objects)
			{
				// Rendering target
				DEFAULT,

				// Viewport
				{ 0, 0, WIDTH, HEIGHT },

				// Color to clear buffers
				{ },

				// Depth test
				{ },

				// Objects
				{
					{
						PLANE,
						MATERIAL_CHECKERBOARD,
						glm::translate(glm::scale(glm::mat4(1.0f), {10.0f, 1.0f, 10.0f}), {0.0f, -1.01f, 0.0f})
					},
					{
						SPHERE,
						MATERIAL_PBR_PHONG,
						glm::translate(glm::mat4(1.0f), {-3.0f, 0.0f, 0.0f})
					},
					{
						TEAPOT,
						MATERIAL_CARTOON
					},
					{
						CUBE,
						MATERIAL_DISCARD,
						glm::translate(glm::mat4(1.0f), {3.0f, 0.0f, 0.0f})
					}
				},

			// The camera
			{
				{ cg::CameraType::ORBITING, { 0.0f, 2.0f, 10.0f }, { 0.0f, 1.0f, 0.0f }, 75.0f }
			},

			// Lights
			{
				{ {0.0f, 5.0f, 2.0f} }
			},

			// Textures
			{

			},

			// Setup uniforms in the shader
			NULL
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