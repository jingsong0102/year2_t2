/*!*****************************************************************************
\file unittests_functions.h
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong\@digipen.edu)
\par Course: CSD2151
\par Assignment: 3.1 (RayTracing)
\date 27/01/2024
\brief This file has declarations of functions for unit tests.

This code is intended to be completed and submitted by a student,
so any changes in this file will be used in the evaluation on the VPL server.
You should not change functions' name and parameter types in provided code.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "unittests_data.h"

float time_intersect_ray_sphere(Ray ray, Sphere sphere);

vec3 rayTracing(Ray ray, Sphere sphere, Light light);

#endif