/*!*****************************************************************************
\file unittests_data.h
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 4.1 (BlinnPhong)
\date 27/11/2023
\brief This file has declarations of data types and values for unit tests.

This code is not intended to be submitted by a student, so any changes
in this file will not be used in the evaluation on the VPL server.
*******************************************************************************/
#ifndef DATA_H
#define DATA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp> 

using mat4 = glm::mat4;
using mat3 = glm::mat3;
using vec4 = glm::vec4;
using vec3 = glm::vec3;

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

struct Material 
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};

#endif