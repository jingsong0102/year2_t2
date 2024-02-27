/*!*****************************************************************************
\file unittests_functions.h
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 5.1 (Environment Mapping)
\date 02/05/2022 (MM/DD/YYYY)
\brief This file has declarations of functions for unit tests.

This code is intended to be completed and submitted by a student,
so any changes in this file will be used in the evaluation on the VPL server.
You should not change functions' name and parameter types in provided code.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "unittests_data.h"

/**
 * @brief Generate a checkerboard pattern texture.
 * 
 * This function generates a checkerboard pattern texture based on the given UV coordinates and size.
 * 
 * @param uv The UV coordinates in the range [0, 1].
 * @param size The size of the checkerboard squares.
 * @return A vec4 representing the color of the texture at the given UV coordinates.
 */
vec4 checkerboardTexture(vec2 uv, float size);

/**
 * @brief Convert a 3D vector to UV coordinates.
 * 
 * This function converts a 3D vector to UV coordinates, considering the vector's direction in space.
 * 
 * @param v The input 3D vector.
 * @return A vec2 representing the UV coordinates corresponding to the input vector.
 */
vec2 vec2uv(vec3 v);

#endif