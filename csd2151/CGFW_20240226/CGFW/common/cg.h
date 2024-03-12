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
                struct Instance
                {
                    Material material;
                    const glm::mat4& modeltransform;
                    bool isVisible;

                    Instance(const Material& material = { },
                        const glm::mat4& modeltransform = NOTRANSFORM,
                        bool isVisible = VISIBLE)
                        :
                        material{ material },
                        modeltransform{ modeltransform },
                        isVisible{ isVisible }
                    {}
                };

                GLuint vao;                     // The Vertex Array Object
                std::vector<GLuint> buffers;    // Vertex buffers
                size_t size;
                std::vector<Instance> instances;
                bool isVisible;

                Object(const char* meshName,
                    const Material& material = { },
                    const glm::mat4& modeltransform = NOTRANSFORM,
                    bool isVisible = VISIBLE)
                    :
                    instances{ },
                    isVisible{ isVisible },
                    vao{ 0 },
                    buffers{ },
                    size{ 0 }
                {
                    instances.push_back({ material, modeltransform, VISIBLE });
                    setBuffers(meshName);
                }

                Object(const char* meshName,
                    const std::initializer_list<Instance>& instances,
                    bool isVisible = VISIBLE) 
                    :
                    instances{ instances },
                    isVisible{ isVisible },
                    vao{ 0 },
                    buffers{ },
                    size{ 0 }
                {
                    setBuffers(meshName);
                }

            private:
                void setBuffers(const char* meshName);
            };

            typedef void (*SetUniformCallback)(Program& shader);

            FBO fbo;
            std::vector<int> viewport;
            std::vector<float> clearColor;
            std::vector<bool> enableDepthBuffer;
            std::vector<Object> objects;
            std::vector<Camera> cameras;
            std::vector<Light> lights;
            std::vector<Texture> textures;
            SetUniformCallback setUniformCallback;

            // For old framework
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
                //this->enableDepthBuffer.push_back(enableDepthBuffer);
            }

            // For new framework
            Pass(
                const FBO& fbo = { },
                const std::initializer_list<int>& viewport = { },
                const std::initializer_list<float>& clearColor = { },
                const std::initializer_list<bool>& enableDepthBuffer = { },
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

        void camerasSetType(cg::CameraType cameraType)
        {
            for (Scene::Pass& pass : passes)
                for (Camera& camera : pass.cameras)
                    camera.setType(cameraType);
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