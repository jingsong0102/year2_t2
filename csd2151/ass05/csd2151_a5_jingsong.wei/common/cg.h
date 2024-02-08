#pragma once
#include <vector>
#include <list>
#include <map>
#include <string>

#include "program.h"
#include "fbo.h"
#include "trianglemeshes.h"
#include "material.h"
#include "light.h"
#include "camera.h"

namespace cg 
{
    #define BLACK           { 0.0f, 0.0f, 0.0f, 0.0f }
    #define GREY            { 0.5f, 0.5f, 0.5f, 0.0f }
    #define WHITE           { 1.0f, 1.0f, 1.0f, 0.0f }

    #define DEFAULT         { }

    #define QUAD    "Quad"
    #define PLANE   "Plane"
    #define TORUS   "Torus"
    #define CUBE    "Cube"
    #define SPHERE  "Sphere"
    #define TEAPOT  "Teapot"
    #define SKYBOX  "SkyBox"

    #define WIDTH   800
    #define HEIGHT  600

    #define NOTRANSFORM  glm::mat4(1.0f)

    #define VISIBLE     true
    #define INVISIBLE   false
    #define ENABLE      true
    #define DISABLE     false

    GLuint loadTexture(const char* filename);
    GLuint loadHdrCubeMap(std::initializer_list<const char*> filenames);

    struct Scene
    {
        struct Texture
        {
            GLuint id;
            GLuint channel;
            GLuint target;

            Texture(GLuint id = 0, GLuint channel = GL_TEXTURE0, GLuint target = GL_TEXTURE_2D)
                : id{ id }, channel{ channel }, target{ target }
            {
            }

            Texture(const std::initializer_list<const char*> filenames, GLuint channel = GL_TEXTURE0);
        };

        struct Pass
        {
            struct Object
            {
                GLuint vao;                     // The Vertex Array Object
                std::vector<GLuint> buffers;    // Vertex buffers
                size_t size;
                Material material;
                const glm::mat4& modeltransform;
                bool isVisible;

                Object(const char* meshName,
                    const Material& material = { },
                    const glm::mat4& modeltransform = NOTRANSFORM,
                    bool isVisible = VISIBLE);
            };

            typedef void (*SetUniformCallback)(Program& shader);

            FBO fbo;
            std::vector<int> viewport;
            std::vector<float> clearColor;
            bool enableDepthBuffer;
            std::vector<Object> objects;
            std::vector<Camera> cameras;
            std::vector<Light> lights;
            std::vector<Texture> textures;
            SetUniformCallback setUniformCallback;

            Pass(       
                const FBO& fbo = { },
                const std::initializer_list<int>& viewport = { },
                const std::initializer_list<float>& clearColor = { },
                bool enableDepthBuffer = true,
                const std::initializer_list<Object>& objects = { },
                const std::initializer_list<Camera>& cameras = { },
                const std::initializer_list<Light>& lights = { },
                const std::initializer_list<Texture> textures = { },
                SetUniformCallback setUniformCallback = nullptr
                )
                : 
                fbo{ fbo }, 
                viewport{ viewport }, 
                clearColor{ clearColor },
                enableDepthBuffer{ enableDepthBuffer }, 
                objects{ objects }, 
                cameras{ cameras }, 
                lights{ lights },
                textures{ textures }, 
                setUniformCallback{ setUniformCallback }
            {
            }

            void resize(int width, int height)
            {
                viewport[2] = width;
                viewport[3] = height;
                fbo.resize(width, height);
            }
        };
        Program shader;
        std::vector<Pass> passes;

        Scene(
            const char * sv = "",
            const char * sf = "",
            const std::initializer_list<Pass>& passes = { }
            )
            : 
            shader(sv, sf), 
            passes{passes}
        {
        }

        Scene(
            GLuint shaderHandle = 0,
            const std::initializer_list<Pass>& passes = { }
            )
            :
            shader(shaderHandle),
            passes{ passes }
        {
        }

        void render();

        void resize(int width, int height)
        {
            for (Scene::Pass& pass : passes)
                pass.resize(width, height);
        }

        void camerasOnCursor(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            for (Scene::Pass& pass : passes)
                for (Camera& camera : pass.cameras)
                    camera.onCursor(xoffset, yoffset, pProgram);
        }

        void lightsOnCursor(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            int i = 0;
            for (Scene::Pass& pass : passes)
                for (Light& light : pass.lights)
                    light.onCursor(xoffset, yoffset, pProgram, i++);
        }

        void camerasOnScroll(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            for (Scene::Pass& pass : passes)
                for (Camera& camera : pass.cameras)
                    camera.onScroll(xoffset, yoffset, pProgram);
        }

        void lightsOnScroll(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            int i = 0;
            for (Scene::Pass& pass : passes)
                for (Light& light : pass.lights)
                    light.onScroll(xoffset, yoffset, pProgram, i++);
        }
    };
}