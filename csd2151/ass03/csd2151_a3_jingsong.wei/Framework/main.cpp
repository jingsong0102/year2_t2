/*!
@file main.cpp
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #3.2
@date 1/28/2024
@brief The Ray Tracing demo assignment.
*/
#include <iostream>

#include "common/debug.h"
#include "common/cg.h"

#include <GLFW/glfw3.h>

cg::Scene* pScene = nullptr;
glm::vec2 screen = { 0, 0 };
bool isNormal = true;
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
        if (key == GLFW_KEY_1)
        {
            isNormal = true;
        }
        if (key == GLFW_KEY_2)
        {
            isNormal = false;
        }

}

/*
   This function serves as the callback parameter for
      glfwSetMouseButtonCallback function used in the main function
*/
void mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{

}

/*
   This function serves as the callback parameter for
      glfwSetCursorPosCallback function used in the main function
*/
void cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos)
{

}

/*
   This function serves as the callback parameter for
      glfwSetScrollCallback function used in the main function
*/
void scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
{

}

/*
   This function serves as the callback parameter for
      glfwSetWindowSizeCallback function used in the main function
*/
void sizeCallback(GLFWwindow* pWindow, int width, int height)
{

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

    GLFWwindow* pWindow = glfwCreateWindow(WIDTH, HEIGHT, "RayTracing", NULL, NULL);
    screen = { WIDTH, HEIGHT };
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
    std::cout << "The Ray Tracing demo." << std::endl;
    std::cout << "Description:" << std::endl;
    std::cout << "   This app utilizes a single-pass full-screen-map rendering process" << std::endl;
    std::cout << "   with vertex and fragment shaders that implement the ray tracing." << std::endl;
    std::cout << "   The object shade is calculated based on the normal vector" << std::endl;
    std::cout << "   orientation (set by default) or based on the light position." << std::endl;
    std::cout << "Interactions:" << std::endl;
    std::cout << "   - Interaction with the object (sphere) is not implemented." << std::endl;
    std::cout << "   - To switch between rendering options, use keyboard keys 1 and 2." << std::endl;
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
        cg::Scene scene =
        {
            // Vertex shader
            {
                #include "RayTracing.vert.glsl"
            },

            // Fragment shader
            {
                #include "RayTracing.frag.glsl"
            },

            // Passes
            {
                // Pass 0
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
                            QUAD
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
                    [](cg::Program& shader)
                    {
                        shader.setUniform("iResolution", screen);
                        shader.setUniform("isNormal", isNormal);
                    }
                }
            }
        };

        pScene = &scene;

        while (!glfwWindowShouldClose(pWindow))
        {
            checkForOpenGLError(__FILE__, __LINE__);
            //update viewport
            int width, height;
            glfwGetFramebufferSize(pWindow, &width, &height);
            screen = { width, height };
            pScene->passes[0].viewport[2] = (int)screen.x;
            pScene->passes[0].viewport[3] = (int)screen.y;
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