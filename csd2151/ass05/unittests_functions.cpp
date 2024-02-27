/*!*****************************************************************************
\file unittests_functions.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 5.1 (Environment Mapping)
\date 02/05/2022 (MM/DD/YYYY)
\brief This file has definitions of functions for unit tests.

This code is intended to be completed and submitted by a student,
so any changes in this file will be used in the evaluation on the VPL server.
You should not change functions' name and parameter types in provided code.
*******************************************************************************/

#include "unittests_functions.h"

template <typename T>
T floor(T a) { return glm::floor(a); }

template <typename T>
T mod(T a, T b) { return glm::mod(a, b); }

#define UNUSED(x) (void)x;

/**
 * @brief Generate a checkerboard pattern texture.
 * 
 * This function generates a checkerboard pattern texture based on the given UV coordinates and size.
 * 
 * @param uv The UV coordinates in the range [0, 1].
 * @param size The size of the checkerboard squares.
 * @return A vec4 representing the color of the texture at the given UV coordinates.
 */
vec4 checkerboardTexture(vec2 uv, float size)
{
    // Scale the UV coordinates
    uv *= size;

    // Calculate the checker pattern
    int checker = int(floor(uv.x) + floor(uv.y)) % 2;

    // Return black or white based on the checker value
    return checker > 0 ? vec4(1.0f, 1.0f, 1.0f, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
/**
 * @brief Convert a 3D vector to UV coordinates.
 * 
 * This function converts a 3D vector to UV coordinates, considering the vector's direction in space.
 * 
 * @param v The input 3D vector.
 * @return A vec2 representing the UV coordinates corresponding to the input vector.
 */
vec2 vec2uv(vec3 v)
{
    //if z normal < 0
    if (v.z < 0.0f)
    {
        float f = -1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;
        //front wall
        if (x >= -1.0f && x < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + x) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if z normal > 0
    else if (v.z > 0.0f)
    {
        float f = 1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;
        //front wall
        if (x > -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - x) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if x normal <0
    if (v.x < 0.0f)
    {
        float f = -1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;
        //front wall
        if (z > -1.0f && z <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - z) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if x normal >0
    else if (v.x > 0.0f)
    {
        float f = 1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;
        //front wall
        if (z >= -1.0f && z < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + z) / 2.0f, (1.0f + y) / 2.0f);
    }
    return vec2(0.0f, 0.0f);
}