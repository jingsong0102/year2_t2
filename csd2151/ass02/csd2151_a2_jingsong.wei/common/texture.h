#pragma once
//#include <GL/glew.h>
//#include "stb_image.h"
//
//namespace cg
//{
//
//    class Texture
//    {
//    public:
//        GLuint id;
//
//        Texture(GLuint id = 0)
//            : id{ id }
//        {
//        }
//
//        Texture(const char* filename)
//            : id{ 0 }
//        {
//            id = loadTexture(filename);
//        }
//
//        Texture(std::initializer_list<const char*> filenames)
//            : id{ 0 }
//        {
//            if (filenames.size() == 6)
//                id = loadHdrCubeMap(
//                    *(filenames.begin()),
//                    *(filenames.begin() + 1),
//                    *(filenames.begin() + 2),
//                    *(filenames.begin() + 3),
//                    *(filenames.begin() + 4),
//                    *(filenames.begin() + 5));
//        }
//    private:
//
//        GLuint loadTexture(const char* filename)
//        {
//            int width, height, bytesPerPix;
//            ;
//            stbi_set_flip_vertically_on_load(true);
//
//            unsigned char* data = stbi_load(filename, &width, &height, &bytesPerPix, 4);
//
//            GLuint tex = 0;
//            if (data)
//            {
//                glGenTextures(1, &tex);
//                glBindTexture(GL_TEXTURE_2D, tex);
//                glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
//                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//                stbi_image_free(data);
//            }
//
//            return tex;
//        }
//
//        GLuint loadHdrCubeMap(const char* filename1,
//            const char* filename2,
//            const char* filename3,
//            const char* filename4,
//            const char* filename5,
//            const char* filename6
//        ) {
//            GLuint texID;
//            glGenTextures(1, &texID);
//            glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
//
//            GLint w, h;
//
//            // Load the first one to get width/height
//            float* data = stbi_loadf(filename1, &w, &h, NULL, 3);
//
//            // Allocate immutable storage for the whole cube map texture
//            glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB32F, w, h);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            data = stbi_loadf(filename2, &w, &h, NULL, 3);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            data = stbi_loadf(filename3, &w, &h, NULL, 3);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            data = stbi_loadf(filename4, &w, &h, NULL, 3);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            data = stbi_loadf(filename5, &w, &h, NULL, 3);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            data = stbi_loadf(filename6, &w, &h, NULL, 3);
//            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
//            stbi_image_free(data);
//
//            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//            return texID;
//        }
//
//    };
//}