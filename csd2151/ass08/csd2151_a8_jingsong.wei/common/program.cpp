#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "program.h"

namespace cg
{

    const char* getTypeString(GLenum type) 
    {
        // There are many more types than are covered here, but
        // these are the most common in these examples.
        switch (type) {
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_DOUBLE:
            return "double";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_INT:
            return "unsigned int";
        case GL_BOOL:
            return "bool";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        default:
            return "?";
        }
    }

    Program::Program(GLuint handle)
        : handle(handle), linked(false)
    {
    }

    Program::Program(const char* sv, const char* sf)
        : handle(0), linked(false)
    {
        if (sv && sf && strlen(sv) && strlen(sf))
        {
            compileShader(sv, GL_VERTEX_SHADER);
            compileShader(sf, GL_FRAGMENT_SHADER);
            link();
        }
    }

    Program::~Program()
    {
        if (!handle || !linked)
            return;
        detachAndDeleteShaderObjects();
        // Delete the program
        glDeleteProgram(handle);
    }

    void Program::detachAndDeleteShaderObjects()
    {
        // Detach and delete the shader objects (if they are not already removed)
        GLint numShaders = 0;
        glGetProgramiv(handle, GL_ATTACHED_SHADERS, &numShaders);
        std::vector<GLuint> shaderNames(numShaders);
        glGetAttachedShaders(handle, numShaders, NULL, shaderNames.data());
        for (GLuint shader : shaderNames)
        {
            glDetachShader(handle, shader);
            glDeleteShader(shader);
        }
    }

    void Program::compileShaderFile(const char* fileName, GLenum type)
    {
        std::ifstream inFile(fileName, std::ios::in);
        if (!inFile)
            throw std::runtime_error(std::string("Unable to open: ") + fileName);

        // Get file contents
        std::stringstream code;
        code << inFile.rdbuf();
        inFile.close();

        compileShader(code.str().c_str(), type);
    }

    void Program::compileShader(const char* code, GLenum type)
    {
        if (!handle)
        {
            handle = glCreateProgram();
            if (!handle)
                throw std::runtime_error("Unable to create shader program.");
        }

        GLuint shaderHandle = glCreateShader(type);

        glShaderSource(shaderHandle, 1, &code, NULL);

        // Compile the shader
        glCompileShader(shaderHandle);

        // Check for errors
        int result;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
        if (GL_FALSE == result)
        {
            // Compile failed, get log
            std::string msg = "Shader compilation failed.\n";

            int length = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                std::string log(length, ' ');
                int written = 0;
                glGetShaderInfoLog(shaderHandle, length, &written, &log[0]);
                msg += log;
            }
            throw std::runtime_error(msg);
        }
        else
        {
            // Compile succeeded, attach shader
            glAttachShader(handle, shaderHandle);
        }
    }

    void Program::link()
    {
        if (!handle)
            throw std::runtime_error("Program has not been compiled.");

        glLinkProgram(handle);
        int status = 0;
        std::string errString;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        if (GL_FALSE == status)
        {
            // Store log and return false
            int length = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
            errString += "Program link failed:\n";
            if (length > 0)
            {
                std::string log(length, ' ');
                int written = 0;
                glGetProgramInfoLog(handle, length, &written, &log[0]);
                errString += log;
            }
        }
        else
        {
            findUniformLocations();
            linked = true;
        }

        detachAndDeleteShaderObjects();

        if (GL_FALSE == status)
            throw std::runtime_error(errString);
    }

    void Program::findUniformLocations()
    {
        uniformLocations.clear();

        GLint numUniforms = 0;

        // For OpenGL 4.3 and above, use glGetProgramResource
        glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

        for (GLint i = 0; i < numUniforms; ++i)
        {
            GLint results[4];
            glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

            if (results[3] != -1) continue;  // Skip uniforms in blocks
            GLint nameBufSize = results[0] + 1;
            char* name = new char[nameBufSize];
            glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name);
            uniformLocations[name] = results[2];
            delete[] name;
        }
    }

    void Program::use() const
    {
        glUseProgram(handle);
    }


    void Program::disuse() const
    {
        glUseProgram(0);
    }

    void Program::printActiveUniforms()
    {
        // For OpenGL 4.3 and above, use glGetProgramResource
        GLint numUniforms = 0;
        glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

        printf("Active uniforms:\n");
        for (int i = 0; i < numUniforms; ++i) {
            GLint results[4];
            glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

            if (results[3] != -1) continue;  // Skip uniforms in blocks
            GLint nameBufSize = results[0] + 1;
            char* name = new char[nameBufSize];
            glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name);
            printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
            delete[] name;
        }
    }

    void Program::printActiveUniformBlocks()
    {
        GLint numBlocks = 0;

        glGetProgramInterfaceiv(handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
        GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
        GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };
        GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

        for (int block = 0; block < numBlocks; ++block) {
            GLint blockInfo[2];
            glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
            GLint numUnis = blockInfo[0];

            char* blockName = new char[blockInfo[1] + 1];
            glGetProgramResourceName(handle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
            printf("Uniform block \"%s\":\n", blockName);
            delete[] blockName;

            GLint* unifIndexes = new GLint[numUnis];
            glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);

            for (int unif = 0; unif < numUnis; ++unif) {
                GLint uniIndex = unifIndexes[unif];
                GLint results[3];
                glGetProgramResourceiv(handle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

                GLint nameBufSize = results[0] + 1;
                char* name = new char[nameBufSize];
                glGetProgramResourceName(handle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
                printf("    %s (%s)\n", name, getTypeString(results[1]));
                delete[] name;
            }

            delete[] unifIndexes;
        }
    }

    void Program::printActiveAttribs()
    {
        // >= OpenGL 4.3, use glGetProgramResource
        GLint numAttribs;
        glGetProgramInterfaceiv(handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

        GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

        printf("Active attributes:\n");
        for (int i = 0; i < numAttribs; ++i) {
            GLint results[3];
            glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

            GLint nameBufSize = results[0] + 1;
            char* name = new char[nameBufSize];
            glGetProgramResourceName(handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
            printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
            delete[] name;
        }
    }

    void Program::validate()
    {
        if (linked)
            throw std::runtime_error("Program is not linked");

        GLint status;
        glValidateProgram(handle);
        glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

        if (GL_FALSE == status) {
            // Store log and return false
            int length = 0;
            std::string logString;

            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

            if (length > 0) {
                char* c_log = new char[length];
                int written = 0;
                glGetProgramInfoLog(handle, length, &written, c_log);
                logString = c_log;
                delete[] c_log;
            }

            throw std::runtime_error(std::string("Program failed to validate\n") + logString);

        }
    }
}