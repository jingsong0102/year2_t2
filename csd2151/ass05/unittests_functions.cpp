/*!*****************************************************************************
\file unittests_functions.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author YOUR NAME (DIGIPEN ID)
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

/*
    This function implements a procedural texture mapping to be uses to apply a checkerboard-like texture to a surface.
First parameter defines the texture coordinates in continues range from 0 to 1.
Second parameter define the number of black and white squares along the texture's width or height.
The first square, started from the corner (0, 0), must be colored black.
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
/*
You have to implement a function that can solve a simplified version of the cube mapping problem, when only back faces of the skybox (which is a cube) are mapped, the other two (top and bottom) are not used. Each internal face of the cube has the same texture coordinates from (0, 0) to (1, 1) at the bottom-left and top-right corners of the face.
Given:
a camera located at the center of a reference frame,
normalized view vector (v) of the camera,
skybox - an axis aligned cube with extreme corners  (-1,-1, -1) and (1, 1, 1).
The function must calculate and return the texture coordinate where the given view ray (defined by vector v) intersects the side of the skybox.
If the view ray intersects the top or bottom sides of the cube, then the function must return vec2(0.0f, 0.0f).
Solution for this problem involves a simple math for 4 different orientations of the vector and is subject to discuss in the class.
*/
vec2 vec2uv(vec3 v)
{
    if (v.z < 0.0f)
    {
        float f = -1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;

        if (x >= -1.0f && x < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + x) / 2.0f, (1.0f + y) / 2.0f);
    }
    else if (v.z > 0.0f)
    {
        float f = 1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;

        if (x > -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - x) / 2.0f, (1.0f + y) / 2.0f);
    }

    if (v.x < 0.0f)
    {
        float f = -1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;

        if (z > -1.0f && z <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - z) / 2.0f, (1.0f + y) / 2.0f);
    }
    else if (v.x > 0.0f)
    {
        float f = 1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;

        if (z >= -1.0f && z < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + z) / 2.0f, (1.0f + y) / 2.0f);
    }

    return vec2(0.0f, 0.0f);
}