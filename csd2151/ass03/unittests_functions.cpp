/*!*****************************************************************************
\file unittests_functions.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong\@digipen.edu)
\par Course: CSD2151
\par Assignment: 3.1 (RayTracing)
\date 27/01/2024
\brief This file has definitions of functions for unit tests.

This code is intended to be completed and submitted by a student,
so any changes in this file will be used in the evaluation on the VPL server.
You should not change functions' name and parameter types in provided code.
*******************************************************************************/

#include "unittests_functions.h"

#include <glm/gtx/extended_min_max.hpp>

#define UNUSED(x) (void)x;

template<typename T>
    T min(T a, T b) { return glm::min(a, b); }

template<typename T>
    T max(T a, T b) { return glm::max(a, b); }

template<typename T>
    T abs(T a) { return glm::abs(a); }

#include "unittests_data.h"

// Return the shortest non-negative time of intersection the ray and the sphere.
// Return 0.0f when there is no intersection
float time_intersect_ray_sphere(Ray ray, Sphere sphere)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return 0.0f; // No intersection
    }

    // Find the nearest positive root
    float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0f * a);

    if (t1 > 0) return t1;
    if (t2 > 0) return t2;
    return 0.0f; // Both roots are negative, no visible intersection

    
}

// Calculate the shade value and use it to set the traced color.
// Return vec3(0.0f, 0.0f, 0.0f), when the ray hits the background.
// Otherwise, calculate the shade value based on the normal and the light source
// position using Lambert's Cosine Law. Set up the value as R, G, and B components 
// of the return value. 
// To apply an ambient light contribution set 0.1f as the smallest value of the shade 
vec3 rayTracing(Ray ray, Sphere sphere, Light light)
{
    float t = time_intersect_ray_sphere(ray, sphere);
    if (t == 0.0f) {
        return vec3(0.0f); // Ray hits the background
    }

    // Find the point of intersection
    vec3 intersection = ray.origin + t * ray.direction;
    vec3 normal = normalize(intersection - sphere.center);
    vec3 light_dir = normalize(light.position - intersection);

    // Lambertian reflectance
    float diffuse = max(dot(normal, light_dir), 0.0f);
    vec3 color = vec3(diffuse); // Assuming white color for simplicity

    // Adding ambient light
    vec3 ambient = vec3(0.1f);
    color = max(color, ambient);

    return color; 

}