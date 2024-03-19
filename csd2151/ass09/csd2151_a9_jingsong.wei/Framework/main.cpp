/*!*****************************************************************************
\file main.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 9 (FiltersAssignment App)
\date 03/17/2024 (MM/DD/YYYY)
\brief This file has definitions of the main function
	   along with global variables and sub-functions used in main.
*******************************************************************************/
#include <iostream>
#include <iomanip>
#include "common/debug.h"
#include "common/cg.h"

#include <GLFW/glfw3.h>

cg::Scene* pScene = nullptr;
float edgeThreshold = 0.03f;
glm::vec2 screen = { 0, 0 };
bool lbutton_down = false;
bool mode_shift = false;
bool mode_alt = false;
int mode = 4;
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
		else if (key == GLFW_KEY_TAB)
		{
			int n = 0;
			for (; n < pScene->passes[0].objects.size(); ++n)
				if (pScene->passes[0].objects[n].isVisible)
					break;

			pScene->passes[0].objects[n].isVisible = false;
			pScene->passes[0].objects[(n + 1 == pScene->passes[0].objects.size()) ? 0 : n + 1].isVisible = true;
		}
		else if (key == GLFW_KEY_1)
		{
			//change pass number
			mode = 1;
			glfwSetWindowTitle(pWindow, "FiltersAssignment - No filters");
		}
		else if (key == GLFW_KEY_2)
		{
			mode = 2;
			glfwSetWindowTitle(pWindow, "FiltersAssignment - Blur filter");
		}
		else if (key == GLFW_KEY_3)
		{
			mode = 3;
			glfwSetWindowTitle(pWindow, "FiltersAssignment - Edge detection filter");
		}
		else if (key == GLFW_KEY_4)
		{
			mode = 4;
			glfwSetWindowTitle(pWindow, "FiltersAssignment - Blur and edge detection filters");
		}
	}
	mode_shift = (mods == GLFW_MOD_SHIFT);
	mode_alt = (mods == GLFW_MOD_ALT);
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
	else if (mode_alt)
	{
		edgeThreshold -= static_cast<float>(yoffset) * 0.01f;
		if (edgeThreshold < 0.01f)
			edgeThreshold = 0.01f;
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << edgeThreshold;
		std::string threshold = stream.str();
		std::string title = "FiltersAssignment - Threshold: " + threshold;
		glfwSetWindowTitle(pWindow, title.c_str());
	}
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
	pScene->passes[2].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
	pScene->passes[3].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
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

	GLFWwindow* pWindow = glfwCreateWindow(WIDTH, HEIGHT, "FiltersAssignment - Blur and edge detection filters", NULL, NULL);
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
	std::cout << "The FiltersAssignment demo." << std::endl;
	std::cout << "Description:" << std::endl;
	std::cout << "   This app demonstrates a multiple-pass mesh rendering process with" << std::endl;
	std::cout << "   filtering using the Gaussian blur and the edge detection filters." << std::endl;
	std::cout << "Interactions:" << std::endl;
	std::cout << "   - Press 1, 2, 3 or 4 to switch between rendering modes." << std::endl;
	std::cout << "   - Press W or O to switch between the camera movement types (orbiting or walking)." << std::endl;
	std::cout << "   - Press the Tab key to change the object." << std::endl;
	std::cout << "   - Hold the Alt key and rotate the mouse wheel to change the edge filtering threshold." << std::endl;
	std::cout << "   - The camera movement is controlled by the mouse left button and wheel." << std::endl;
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
		GLuint texture = 0;
		GLuint texture1 = 0;
		GLuint texture2 = 0;

		cg::Scene scene =
		{
			// Vertex shader
			{
				#include "FiltersAssignment.vert.glsl"
			},

			// Fragment shader
			{
				#include "FiltersAssignment.frag.glsl"
			},

			// Passes
			{
				// Pass 0
				{
					// Rendering target
					{ cg::FBO::Type::Texture, WIDTH, HEIGHT, &texture1 },

					// Viewport
					{ 0, 0, WIDTH, HEIGHT },

					// Color to clear buffers
					BLACK,

					// Depth test
					ENABLE,

					// Objects
					{
						{
							TEAPOT,
							MATERIAL_OBJECT,
							NOTRANSFORM,
							VISIBLE
						},
						{
							TORUS,
							MATERIAL_OBJECT,
							NOTRANSFORM,
							INVISIBLE
						},
						{
							CUBE,
							MATERIAL_OBJECT,
							NOTRANSFORM,
							INVISIBLE
						},
						{
							SPHERE,
							MATERIAL_OBJECT,
							NOTRANSFORM,
							INVISIBLE
						}
					},

			// The camera
			{
				DEFAULT
			},

			// Lights
			{
				DEFAULT
			},

			// Textures
			{

			},

			// Setup uniforms in the shader
			NULL
		},

			// Pass 1
				{
					// Target
					{ cg::FBO::Type::Texture, WIDTH, HEIGHT, &texture2 },

					// Viewport
					{ 0, 0, WIDTH, HEIGHT },

					// Color to clear buffers
					{ },

					// Depth test
					ENABLE,

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
				{ texture1 }
			},

			// Setup uniforms in the shader
			NULL
		},

			// Pass 2
			{
				// Rendering target
				{ cg::FBO::Type::Texture, WIDTH, HEIGHT, &texture },

				// Viewport
				{ 0, 0, WIDTH, HEIGHT },

				// Color to clear buffers
				{ },

				// Depth test
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
				{ texture2 }
			},

			// Setup uniforms in the shader
			NULL
		},
			// Pass 3
					{
						// Rendering target
						DEFAULT,

						// Viewport
						{ 0, 0, WIDTH, HEIGHT },

						// Color to clear buffers
						{ },

						// Depth test
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
				{ texture }
			},

			// Setup uniforms in the shader
			[](cg::Program& shader)
			{
					shader.setUniform("EdgeThreshold", edgeThreshold);
					shader.setUniform("mode", mode);
			}
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