/*!*****************************************************************************
\file unittests_functions.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong\@digipen.edu)
\par Course: CSD2151
\par Assignment: 4.1 (BlinnPhong)
\date 04/02/2024
\brief This file has definitions of functions for unit tests for
BlinnPhong method.

This code is intended to be completed and submitted by a student,
so any changes in this file will be used in the evaluation on the VPL server.
You should not change functions' name and parameter types in provided code.
*******************************************************************************/

#include "unittests_functions.h"
#include <glm/gtx/extended_min_max.hpp>

template <typename T>
T min(T a, T b) { return glm::min(a, b); }

template <typename T>
T max(T a, T b) { return glm::max(a, b); }

template <typename T>
T abs(T a) { return glm::abs(a); }

template <typename T>
float length(T a) { return glm::length(a); }

template <typename T>
float dot(T a, T b) { return glm::dot(a, b); }

template <typename T>
T pow(T a, T b) { return glm::pow(a, b); }

#define UNUSED(x) (void)x;

/*
    Function implements the Blinn-Phong shading method of shading
    Parameters:
        position - coordinates of a fragment in the view space,
        normal - the normal vector of the fragment,
        light - the light info,
        material - the material info,
        view - the view transform matrix to transfom the light position to view space.
    Returns:
        - color components of the fragment based on the fragment position and normal vector,
          the light position and properties, and the material properties
        - vec3(0.0f) if the light position in the view space and position are the same points.
    Assume:
        - the camera and the fragment positions are never the same points.
*/
vec3 BlinnPhong(vec3 position, vec3 normal, Light light, Material material, mat4 view)
{
    // calculate normal
    normal = length(normal) > 0.0f ? normalize(normal) : vec3(0.0f, 0.0f, 1.0f);
    // calculate light position in the view space
    vec4 lightPosInView = view * vec4(light.position, 1.0);
    vec3 lightDir = vec3(lightPosInView) - position;
    if (length(lightDir) > 0.0f)
        lightDir = normalize(lightDir);
    else
        return vec3(0.0f);
    // calculate view direction
    vec3 viewDir = normalize(-position);
    // calculate halfway direction
    vec3 halfwayDir = lightDir + viewDir;
    if (length(halfwayDir) > 0.0f)
        halfwayDir = normalize(halfwayDir);
    else
        return vec3(0.0f);
    // calculate light
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.Ld * (diff * material.Kd);
    // calculate specular
    float specAngle = max(dot(normal, halfwayDir), 0.0f);
    float spec = diff > 0.0f ? pow(specAngle, material.shininess) : 0.0f;
    vec3 specular = light.Ls * (spec * material.Ks);
    // calculate ambient
    vec3 ambient = light.La * material.Ka;

    return ambient + diffuse + specular;
}