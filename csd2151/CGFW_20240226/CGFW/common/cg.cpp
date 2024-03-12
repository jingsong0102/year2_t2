#include <stdexcept>
#include <sstream>   
#include <fstream>

#include "cg.h"

// For loading textures
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace cg
{

    GLuint loadTexture(const char* filename)
    {
        int width, height, bytesPerPix;
 
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(filename, &width, &height, &bytesPerPix, 4);
        
        if (!data)
            throw std::runtime_error(std::string("file ") + filename + " not found.");

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        stbi_image_free(data);

        return tex;
    }

    GLuint loadHdrCubeMap(const std::initializer_list<const char*> filenames)
    {
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

        GLuint i = 0;
        for (const char* filename : filenames)
        {
            GLint w, h;
            // Load the first one to get width/height
            float* data = stbi_loadf(filename, &w, &h, NULL, 3);

            if (!data)
                throw std::runtime_error(std::string("file ") + filename + " not found.");

            if (i == 0)
                // Allocate immutable storage for the whole cube map texture
                glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB32F, w, h);

            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);

            stbi_image_free(data);
            i++;
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texID;
    }

    Scene::Texture::Texture(const std::initializer_list<const char*> filenames, GLuint channel /*= GL_TEXTURE0*/)
        : id{ 0 }, channel{ channel }, target{ 0 }
    {
        if (filenames.size() == 1)
        {
            id = loadTexture(*filenames.begin());
            target = GL_TEXTURE_2D;
        }
        else if (filenames.size() == 6)
        {
            id = loadHdrCubeMap(filenames);
            target = GL_TEXTURE_CUBE_MAP;
        }
    }

    void Scene::Pass::Object::setBuffers(const char* meshName)
    {
        TriangleMesh mesh(meshName);

        // Must have data for indices, points, and normals
        if (mesh.indices.empty() || mesh.points.empty() || mesh.normals.empty())
            return;

        size = mesh.indices.size();

        GLuint indexBuf = 0, posBuf = 0, normBuf = 0, tcBuf = 0, tangentBuf = 0;
        glGenBuffers(1, &indexBuf);
        buffers.push_back(indexBuf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &posBuf);
        buffers.push_back(posBuf);
        glBindBuffer(GL_ARRAY_BUFFER, posBuf);
        glBufferData(GL_ARRAY_BUFFER, mesh.points.size() * sizeof(GLfloat), mesh.points.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &normBuf);
        buffers.push_back(normBuf);
        glBindBuffer(GL_ARRAY_BUFFER, normBuf);
        glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(GLfloat), mesh.normals.data(), GL_STATIC_DRAW);

        if (!mesh.texCoords.empty())
        {
            glGenBuffers(1, &tcBuf);
            buffers.push_back(tcBuf);
            glBindBuffer(GL_ARRAY_BUFFER, tcBuf);
            glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(GLfloat), mesh.texCoords.data(), GL_STATIC_DRAW);
        }

        if (!mesh.tangents.empty())
        {
            glGenBuffers(1, &tangentBuf);
            buffers.push_back(tangentBuf);
            glBindBuffer(GL_ARRAY_BUFFER, tangentBuf);
            glBufferData(GL_ARRAY_BUFFER, mesh.tangents.size() * sizeof(GLfloat), mesh.tangents.data(), GL_STATIC_DRAW);
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);

        // Position
        glBindBuffer(GL_ARRAY_BUFFER, posBuf);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // Normal
        glBindBuffer(GL_ARRAY_BUFFER, normBuf);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        // Tex coords
        if (!mesh.texCoords.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, tcBuf);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(2);
        }

        // Tangent
        if (!mesh.tangents.empty())
        {
            glBindBuffer(GL_ARRAY_BUFFER, tangentBuf);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(3);
        }

        glBindVertexArray(0);
    }

    void Scene::render()
    {
        int i = 0;
        for (const Scene::Pass& pass : passes)
        {
            // Set the target
            glBindFramebuffer(GL_FRAMEBUFFER, pass.fbo.handle);

            // Set the pass number and other parameters for rendering if any
            shader.use();
            shader.setUniform("Pass", i++);
            if (pass.setUniformCallback)
                pass.setUniformCallback(shader);
            shader.disuse();

            // Set the viewport
            if (pass.viewport.size() == 4)
                glViewport(pass.viewport[0], pass.viewport[1], pass.viewport[2], pass.viewport[3]);

            // Cleaning the target
            if (pass.clearColor.size() == 4)
            {
                glClearColor(pass.clearColor[0], pass.clearColor[1], pass.clearColor[2], pass.clearColor[3]);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            // Set and clear the depth buffer
            if (pass.enableDepthBuffer.size() > 0)
            {
                GLboolean enableDepthTest;
                glGetBooleanv(GL_DEPTH_TEST, &enableDepthTest);
                if (enableDepthTest)
                    if (pass.enableDepthBuffer[0])
                        glClear(GL_DEPTH_BUFFER_BIT);
                    else
                    {
                        glDisable(GL_DEPTH_TEST);
                        glClear(GL_DEPTH_BUFFER_BIT);
                    }
                else
                    if (pass.enableDepthBuffer[0])
                    {
                        glEnable(GL_DEPTH_TEST);
                        glClear(GL_DEPTH_BUFFER_BIT);
                    }
            }

            // Bind textures for the pass
            for (const Texture& info : pass.textures)
                if (info.id) 
                {
                    glActiveTexture(info.channel);
                    glBindTexture(info.target, info.id);
                }

            // Setup lights
            for (int idx = 0; idx < pass.lights.size(); ++idx)
                pass.lights[idx].setUniforms(&shader, idx);


            glm::mat4 V = glm::mat4(1.0f);
            glm::mat4 P = glm::mat4(1.0f);

            if (pass.cameras.size() == 1)
            {
                pass.cameras[0].setUniforms(&shader);

                if (pass.viewport.size()==4 && pass.viewport[3])
                {
                    V = pass.cameras[0].getLookAt();
                    P = pass.cameras[0].getPerspective((float)pass.viewport[2] / pass.viewport[3]);
                }
            }

            shader.use();
            shader.setUniform("V", V);
            shader.setUniform("P", P);
            shader.disuse();

            for (const Scene::Pass::Object& object : pass.objects)
                if (object.isVisible && !object.instances.empty())
                {
                    for (const Scene::Pass::Object::Instance& instance : object.instances)
                        if (instance.isVisible)
                        {
                            instance.material.setUniforms(&shader);

                            shader.use();
                            shader.setUniform("M", instance.modeltransform);

                            glBindVertexArray(object.vao);
                            glDrawElements(GL_TRIANGLES, (GLsizei)object.size, GL_UNSIGNED_INT, 0);
                            glBindVertexArray(0);

                            shader.disuse();
                        }
                }

            //for (const Scene::Pass::Object& object : pass.objects)
            //    if (object.instances[0].isVisible)
            //    {
            //        object.instances[0].material.setUniforms(&shader);

            //        shader.use();
            //        shader.setUniform("M", object.instances[0].modeltransform);

            //        glBindVertexArray(object.vao);
            //        glDrawElements(GL_TRIANGLES, (GLsizei)object.size, GL_UNSIGNED_INT, 0);
            //        glBindVertexArray(0);

            //        shader.disuse();
            //    }
            glFinish();
        }
    }
}