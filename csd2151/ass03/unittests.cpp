/*!*****************************************************************************
\file unittests.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 3.1 (RayTracing)
\date 24/01/2024
\brief This file has definitions of functions for unit tests that are used for
evaluation the code submitted by students on Moodle as VPL.

This code is NOT intended to be submitted by a student, so any changes
in this file will not be used in the evaluation on the VPL server.
*******************************************************************************/
#include <iostream>
#include <sstream>

#include <glm/gtc/epsilon.hpp>

#include "unittests_functions.h"

const float EPSILON = 0.001f;

bool epsilonEqual(vec3 actual, vec3 expected, float EPSILON)
{
    return  glm::epsilonEqual(actual.x, expected.x, EPSILON) &&
                glm::epsilonEqual(actual.y, expected.y, EPSILON) &&
                    glm::epsilonEqual(actual.z, expected.z, EPSILON);
}

std::ostream& operator<<(std::ostream& os, vec3 const& v)
{
    os << v.x << "," << v.y << "," << v.z;
    return os;
}

// Non evaluated selftest
void test0()
{
    if (!glm::epsilonEqual(0.0f + EPSILON / 2.0f, 0.0f, EPSILON))
        std::cout << "Selftest failed" << std::endl;

    if (glm::epsilonEqual(0.0f + EPSILON * 2.0f, 0.0f, EPSILON))
        std::cout << "Selftest failed" << std::endl;

    if (!glm::epsilonEqual(1000.0f + EPSILON / 2.0f, 1000.0f, EPSILON))
        std::cout << "Selftest failed" << std::endl;

    if (glm::epsilonEqual(10000.0f + EPSILON * 2.0f, 10000.0f, EPSILON))
        std::cout << "Selftest failed" << std::endl;
}

// No ray, no sphere
void test1()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 0.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 0.0f);
    sphere.radius = 0.0f;

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = 0.0f;

    std::cout << "Test 1 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// No ray
void test2()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 0.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, -10.0f);
    sphere.radius = 5.0f;

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = 0.0f;

    std::cout << "Test 2 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Opposite direction - no intersection
void test3()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, -1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 10.0f);
    sphere.radius = 3.0f;

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = 0.0f;

    std::cout << "Test 3 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at shortest time
void test4()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 10.0f);
    sphere.radius = 3.0f;

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = 7.0f;

    std::cout << "Test 4 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at longest time
void test5()
{
    Ray ray;
    ray.origin = vec3(0.0f, 1.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 5.0f);
    sphere.radius = 1.0f;

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = 5.0f;

    std::cout << "Test 5 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at the middle btw shortest and longest time
void test6()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = glm::normalize(vec3(0.0f, 3.0f, 5.0f));

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 8.0f);
    sphere.radius = glm::sqrt(3.0f*3.0f+3.0f*3.0f);

    float actual = time_intersect_ray_sphere(ray, sphere);
    float expected = glm::sqrt(5.0f*5.0f+3.0f*3.0f);

    std::cout << "Test 6 : ";
    if (glm::epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// No intersection, no light.
// The result is the ambient light contribution only.
void test7()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, -1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, -10.0f);
    sphere.radius = 3.0f;

    Light light;
    light.position = vec3(0.0f, 0.0f, -20.0f);

    vec3 actual = rayTracing(ray, sphere, light);
    vec3 expected = vec3(0.1f, 0.1f, 0.1f);       

    std::cout << "Test 7 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at shortest time, both eye and light at origin
void test8()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 10.0f);
    sphere.radius = 3.0f;

    Light light;
    light.position = vec3(0.0f, 0.0f, 0.0f);

    vec3 actual = rayTracing(ray, sphere, light);
    vec3 expected = vec3(1.0f, 1.0f, 1.0f);

    std::cout << "Test 8 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at longest time, the light is in the derection of the normal vector
void test9()
{
    Ray ray;
    ray.origin = vec3(0.0f, 1.0f, 0.0f);
    ray.direction = vec3(0.0f, 0.0f, 1.0f);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 5.0f);
    sphere.radius = 1.0f;

    Light light;
    light.position = vec3(0.0f, 10.0f, 5.0f);

    vec3 actual = rayTracing(ray, sphere, light);
    vec3 expected = vec3(1.0f, 1.0f, 1.0f);

    std::cout << "Test 9 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Intersects at the middle btw shortest and longest time
void test10()
{
    Ray ray;
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.direction = glm::normalize(vec3(0.0f, 3.0f, 5.0f));

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, 8.0f);
    sphere.radius = glm::sqrt(3.0f * 3.0f + 3.0f * 3.0f);

    Light light;
    light.position = vec3(10.0f, 10.0f, 10.0f);

    vec3 actual = rayTracing(ray, sphere, light);
    vec3 expected = vec3(0.107211f, 0.107211f, 0.107211f);

    std::cout << "Test 10 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}