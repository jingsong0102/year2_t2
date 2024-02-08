#pragma once

#include "program.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians

namespace cg
{

    #define PHONG_MATERIAL_BLUE  \
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.3f, 0.5f, 0.9f } },\
            { "material.Kd", { 0.3f, 0.5f, 0.9f } },\
            { "material.Ks", { 0.8f, 0.8f, 0.8f } },\
        }

    #define PHONG_MATERIAL_ORANGE  \
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.9f, 0.5f, 0.3f } },\
            { "material.Kd", { 0.9f, 0.5f, 0.3f } },\
            { "material.Ks", { 0.8f, 0.8f, 0.8f } },\
        }

    #define PHONG_MATERIAL_WHITE  \
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 1.0f, 1.0f, 1.0f } },\
            { "material.Kd", { 1.0f, 1.0f, 1.0f } },\
            { "material.Ks", { 1.0f, 1.0f, 1.0f } },\
        }

    #define MATERIAL_REFLECT_REFRACT  \
        {\
            { "material.color", { 0.5f, 0.5f, 0.5f, 1.0f } },\
            { "material.reflectionFactor", { 0.9f } },\
            { "material.eta", { 0.94f } } \
        }

    #define MATERIAL_PBR  \
        {\
            { "material.rough", { 0.3f } },\
            { "material.color", { 0.1f, 0.1f, 0.1f } }\
        }

    #define MATERIAL_PLANE  PHONG_MATERIAL_BLUE

    #define MATERIAL_OBJECT PHONG_MATERIAL_ORANGE

    // Material base class
    struct Material
    {
        std::map<std::string, std::vector<float>> params;

        Material(const std::initializer_list< std::pair<std::string, std::initializer_list<float>> >& params)
            : params{ }
        {
            for (const std::pair < std::string, std::initializer_list<float>>& p : params)
                this->params.insert(p);
        }

        void setUniforms(Program* pProgram) const
        { 
            pProgram->use();
            for (const std::pair <std::string, std::vector<float>>& p : params)
                pProgram->setUniform(p.first, p.second);
            pProgram->disuse();
        
        }
    };
}