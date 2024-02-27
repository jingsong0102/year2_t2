/*!
@file main.cpp
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #5.2
@date 02/13/2024
@brief The Environment Mapping assignment. This app utilizes
a multi-pass mesh rendering process with vertex and fragment
shaders that implement cube mapping, refraction and reflection.
*/
#include <iostream>

#include "common/debug.h"
#include "common/cg.h"

#include <GLFW/glfw3.h>

cg::Scene *pScene = nullptr;
glm::vec2 screen = {0, 0};
bool lbutton_down = false;
bool mode_shift = false;
bool mode_alt = false;
/*
   This function serves as the callback parameter for
      glfwSetKeyCallback function used in the main function

   Esc - close the app
*/
void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    mode_alt = (mods == GLFW_MOD_ALT);
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(pWindow, GL_TRUE);

        else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
        {
            int value = key - GLFW_KEY_0;
            for (int n = 0; n < pScene->passes[1].objects.size(); ++n)
            {
                if (mode_alt)
                {
                    pScene->passes[1].objects[n].material.params["material.eta"] = {0.9f + value / 90.0f};
                }
                else
                {
                    pScene->passes[1].objects[n].material.params["material.reflectionFactor"] = {value / 9.0f};
                }
            }
        }
    }
}
/*
   This function serves as the callback parameter for
      glfwSetMouseButtonCallback function used in the main function
*/
void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
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
void cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
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
void scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
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
void sizeCallback(GLFWwindow *pWindow, int width, int height)
{
    screen.x = static_cast<float>(width);
    screen.y = static_cast<float>(height);

    pScene->passes[0].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
    pScene->passes[1].resize(static_cast<int>(screen.x), static_cast<int>(screen.y));
}

/*
    The main function
*/
int main(int argc, char **argv)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

#if defined(_DEBUG)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *pWindow = glfwCreateWindow(WIDTH, HEIGHT, "EnvironmentMapAssignment", NULL, NULL);
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

#if defined(_DEBUG)
    dumpGLInfo();
#endif

    std::cout << std::endl;
    std::cout << "The Environment Mapping Assignment demo." << std::endl;
    std::cout << "Description:" << std::endl;
    std::cout << "   This app utilizes a multi-pass mesh rendering process with vertex" << std::endl;
    std::cout << "   and fragment shaders that implement cube mapping, refraction and reflection." << std::endl;
    std::cout << "Interactions:" << std::endl;
    std::cout << "   - Keys 0 to 9 to change the reflection factor." << std::endl;
    std::cout << "   - Alt and keys 0 to 9 to change the ratio of indices of refraction." << std::endl;
    std::cout << "   - The orbiting around the object is controlled by the mouse left button and wheel." << std::endl;
    std::cout << "   - The screen is resizable." << std::endl;
    std::cout << "   - You can press key Esc to close the app." << std::endl;
    std::cout << std::endl;

#if defined(_DEBUG)
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
#include "EnvironmentMap.vert.glsl"
                },

                // Fragment shader
                {
#include "EnvironmentMap.frag.glsl"
                },

                // Passes
                {
                    // Pass 0 (to render background)
                    {
                        // Rendering target
                        DEFAULT,

                        // Viewport
                        {0, 0, WIDTH, HEIGHT},

                        // Color to clear buffers
                        {},

                        // Depth test
                        ENABLE,

                        // Objects
                        {
                            {SKYBOX}},

                        // The camera
                        {
                            {{0.0f, 0.0f, 3.0f}}},

                        // Lights
                        {

                        },

                        // Textures
                        {

                        },

                        // Setup uniforms in the shader
                        NULL},

                    // Pass 1 (to render the object)
                    {
                        // Rendering target
                        DEFAULT,

                        // Viewport
                        {0, 0, WIDTH, HEIGHT},

                        // Color to clear buffers
                        {},

                        // Depth test
                        ENABLE,

                        // Objects
                        {
                            {SPHERE,
                             MATERIAL_REFLECT_REFRACT},
                            {CUBE,
                             MATERIAL_REFLECT_REFRACT,
                             glm::translate(glm::mat4(1.0f), {3.5f, 0.0f, 0.f})},
                            {TORUS,
                             MATERIAL_REFLECT_REFRACT,
                             glm::translate(glm::mat4(1.0f), {-3.5f, 0.0f, 0.f})}},
                        // The camera
                        {
                            {{0.0f, 0.0f, 3.0f}}},

                        // Lights
                        {
                            DEFAULT},

                        // Textures
                        {

                        },

                        // Setup uniforms in the shader
                        NULL}}};

        pScene = &scene;

        while (!glfwWindowShouldClose(pWindow))
        {
            checkForOpenGLError(__FILE__, __LINE__);

            scene.render();

            glfwSwapBuffers(pWindow);
            glfwPollEvents();
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;

        // Close pWindow and terminate GLFW
        glfwTerminate();

        // Exit program
        std::exit(EXIT_FAILURE);
    }

#if defined(_DEBUG)
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

    // Close pWindow and terminate GLFW
    glfwTerminate();

    // Exit program
    return EXIT_SUCCESS;
}