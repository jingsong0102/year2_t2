#pragma once

#include <string>
#include <map>
#include <stdexcept>

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace cg
{

    class Program
    {
    public:
        GLuint handle;
    private:
        bool linked;
        std::map<std::string, int> uniformLocations;

        GLint getUniformLocation(const std::string& name)
        {
            auto pos = uniformLocations.find(name);
            if (pos == uniformLocations.end())
                return uniformLocations[name] = glGetUniformLocation(handle, name.c_str());
            return pos->second;
        }

        void detachAndDeleteShaderObjects();
        void link();

    public:
        Program(GLuint handle);
        Program(const char* sv, const char* sf);

        ~Program();

        Program(const Program& rhs) = delete;
        Program& operator=(const Program& rhs) = delete;

        operator GLuint() 
        {
            return handle;
        }
       
        void compileShaderFile(const char* fileName, GLenum type);
        void compileShader(const char* code, GLenum type);

        void validate();
        void use() const;
        void disuse() const;

        void bindAttribLocation(GLuint location, const char* name) {
            glBindAttribLocation(handle, location, name);
        }

        void bindFragDataLocation(GLuint location, const char* name) {
            glBindFragDataLocation(handle, location, name);
        }

        void setUniform(const std::string& name, float x, float y, float z) {
            glUniform3f(getUniformLocation(name), x, y, z);
        }

        void setUniform(const std::string& name, const glm::vec2& v) {
            glUniform2f(getUniformLocation(name), v.x, v.y);
        }

        void setUniform(const std::string& name, const glm::vec3& v) {
            this->setUniform(name, v.x, v.y, v.z);
        }

        void setUniform(const std::string& name, const glm::vec4& v) {
            glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
        }

        void setUniform(const std::string& name, const std::vector<float>& v) {
            if (v.size() == 1)
                glUniform1f(getUniformLocation(name), v[0]);
            else if (v.size() == 2)
                glUniform2f(getUniformLocation(name), v[0], v[1]);
            else if (v.size() == 3)
                glUniform3f(getUniformLocation(name), v[0], v[1], v[2]);
            else if (v.size() == 4)
                glUniform4f(getUniformLocation(name), v[0], v[1], v[2], v[3]);
        }

        void setUniform(const std::string& name, const glm::mat4& m) {
            glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]);
        }

        void setUniform(const std::string& name, const glm::mat3& m) {
            glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]);
        }

        void setUniform(const std::string& name, float val) {
            glUniform1f(getUniformLocation(name), val);
        }

        void setUniform(const std::string& name, int val) {
            glUniform1i(getUniformLocation(name), val);
        }

        void setUniform(const std::string& name, bool val) {
            glUniform1i(getUniformLocation(name), val);
        }

        void setUniform(const std::string& name, GLuint val) {
            glUniform1ui(getUniformLocation(name), val);
        }

        void findUniformLocations();

        void printActiveUniforms();
        void printActiveUniformBlocks();
        void printActiveAttribs();
    };
}
