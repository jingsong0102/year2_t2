/*!*****************************************************************************
\file material.h
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: all
\date 02/23/2024 (MM/DD/YYYY)
\brief This file has definitions of the material used in the framework
       for scene definitions.
*******************************************************************************/
#pragma once

#include "program.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians

namespace cg
{

    #define PHONG_MATERIAL_BLUE\
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.3f, 0.5f, 0.9f } },\
            { "material.Kd", { 0.3f, 0.5f, 0.9f } },\
            { "material.Ks", { 0.8f, 0.8f, 0.8f } },\
            { "material.effect", { 0.0f } }\
        }

    #define PHONG_MATERIAL_ORANGE\
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.9f, 0.5f, 0.3f } },\
            { "material.Kd", { 0.9f, 0.5f, 0.3f } },\
            { "material.Ks", { 0.8f, 0.8f, 0.8f } },\
            { "material.effect", { 0.0f } }\
        }

    #define PHONG_MATERIAL_WHITE\
        {\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 1.0f, 1.0f, 1.0f } },\
            { "material.Kd", { 1.0f, 1.0f, 1.0f } },\
            { "material.Ks", { 1.0f, 1.0f, 1.0f } },\
            { "material.effect", { 0.0f } }\
        }

    #define MATERIAL_REFLECT_REFRACT\
        {\
            { "material.color", { 0.5f, 0.5f, 0.5f } },\
            { "material.reflectionFactor", { 0.9f } },\
            { "material.eta", { 0.94f } },\
            { "material.effect", { 0.0f } }\
        }

    #define MATERIAL_PROPERTIES_PBR_RED\
            { "material.rough", { 0.3f } },\
            { "material.metal", { 0.0f } },\
            { "material.color", { 1.0f, 0.0f, 0.0f } }

    #define MATERIAL_PROPERTIES_PBR_GREEN\
            { "material.rough", { 0.3f } },\
            { "material.metal", { 0.0f } },\
            { "material.color", { 0.0f, 1.0f, 0.0f } }

    #define MATERIAL_PROPERTIES_PBR_BLUE\
            { "material.rough", { 0.3f } },\
            { "material.metal", { 0.0f } },\
            { "material.color", { 0.0f, 0.0f, 1.0f } }

    #define MATERIAL_PROPERTIES_PBR_GREY\
            { "material.rough", { 0.3f } },\
            { "material.metal", { 0.0f } },\
            { "material.color", { 0.3f, 0.3f, 0.3f } }

    #define MATERIAL_PROPERTIES_PHONG_RED\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.0f, 0.0f, 0.0f } },\
            { "material.Kd", { 1.0f, 0.0f, 0.0f } },\
            { "material.Ks", { 1.0f, 0.0f, 0.0f } }   

    #define MATERIAL_PROPERTIES_PHONG_GREEN\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.0f, 0.0f, 0.0f } },\
            { "material.Kd", { 0.0f, 1.0f, 0.0f } },\
            { "material.Ks", { 0.0f, 1.0f, 0.0f } } 

    #define MATERIAL_PROPERTIES_PHONG_BLUE\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.0f, 0.0f, 0.0f } },\
            { "material.Kd", { 0.0f, 0.0f, 1.0f } },\
            { "material.Ks", { 0.0f, 0.0f, 1.0f } } 

    #define MATERIAL_PROPERTIES_PHONG_GREY\
            { "material.shininess", { 100.0f } },\
            { "material.Ka", { 0.0f, 0.0f, 0.0f } },\
            { "material.Kd", { 0.3f, 0.3f, 0.3f } },\
            { "material.Ks", { 0.3f, 0.3f, 0.3f } } 

    #define MATERIAL_PBR\
            {\
                MATERIAL_PROPERTIES_PBR_RED,\
                { "material.effect", { 0.0f } }\
            }
    
    #define MATERIAL_PBR_PHONG\
            {\
                MATERIAL_PROPERTIES_PBR_RED,\
                MATERIAL_PROPERTIES_PHONG_RED,\
                { "material.effect", { 0.0f } }\
            }

    #define MATERIAL_DISCARD  \
            {\
                MATERIAL_PROPERTIES_PBR_GREEN,\
                MATERIAL_PROPERTIES_PHONG_GREEN,\
                { "material.effect", { 1.0f } }\
            }

    #define MATERIAL_CARTOON  \
            {\
                MATERIAL_PROPERTIES_PBR_BLUE,\
                MATERIAL_PROPERTIES_PHONG_BLUE,\
                { "material.effect", { 2.0f } }\
            }

    #define MATERIAL_CHECKERBOARD  \
            {\
                MATERIAL_PROPERTIES_PBR_GREY,\
                MATERIAL_PROPERTIES_PHONG_GREY,\
                { "material.effect", { 3.0f } }\
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