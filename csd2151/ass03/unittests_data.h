/*!*****************************************************************************
\file unittests_data.h
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 3.1 (RayTracing)
\date 24/01/2024
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
    vec3 position;
};

struct Ray
{
    vec3 origin;
    vec3 direction; // Normalized
};

struct Sphere
{
    vec3 center;
    float radius;
};

#endif