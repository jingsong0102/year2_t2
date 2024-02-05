/*!*****************************************************************************
\file unittests.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 4.1 (BlinnPhong)
\date 27/11/2023
\brief This file has definitions of functions for unit tests that are used for
evaluation the code submitted by students on Moodle as VPL.

This code is not intended to be submitted by a student, so any changes
in this file will not be used in the evaluation on the VPL server.
*******************************************************************************/
#include <iostream>
#include <sstream>

#include "unittests_functions.h"

#include <glm/glm.hpp>

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

// All 0 test
void test1()
{
    vec3 position = { 0.0f, 0.0f, 0.0f };

    vec3 normal = { 0.0f, 0.0f, 0.0f };

    Light light = {
        {0.0f, 0.0f, 0.0f}, // vec3 position;
        {0.0f, 0.0f, 0.0f}, // vec3 La;
        {0.0f, 0.0f, 0.0f}, // vec3 Ld;
        {0.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.0f, 0.0f, 0.0f}, // vec3 Kd;
        {0.0f, 0.0f, 0.0f}, // vec3 Ks;
        0.0f                // float shininess; 
    };

    mat4 view = mat4(0.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.0f, 0.0f, 0.0f };

    std::cout << "Test 1 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Ambient light only
void test2()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 1.0f, 0.0f}, // vec3 position;
        {0.1f, 0.2f, 0.3f}, // vec3 La; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ld; 
        {0.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.1f, 0.2f, 0.3f}, // vec3 Ka;
        {0.0f, 0.0f, 0.0f}, // vec3 Kd; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ks; 
        0.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.1f * 0.1f, 0.2f * 0.2f, 0.3f * 0.3f };

    std::cout << "Test 2 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Diffuse light only, cos of 90 deg
void test3()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 1.0f, -1.0f}, // vec3 position;
        {0.0f, 0.0f,  0.0f}, // vec3 La; 
        {0.1f, 0.2f,  0.3f}, // vec3 Ld; 
        {0.0f, 0.0f,  0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.1f, 0.2f, 0.3f}, // vec3 Kd; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ks; 
        0.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.0f, 0.0f, 0.0f };

    std::cout << "Test 3 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Diffuse light only, cos of 0 deg
void test4()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 0.0f, 0.0f}, // vec3 position;    <---- the difference
        {0.0f, 0.0f, 0.0f}, // vec3 La; 
        {0.1f, 0.2f, 0.3f}, // vec3 Ld; 
        {0.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.1f, 0.2f, 0.3f}, // vec3 Kd; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ks; 
        0.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.1f * 0.1f, 0.2f * 0.2f, 0.3f * 0.3f };

    std::cout << "Test 4 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Diffuse light only, cos of 45 deg
void test5()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 1.0f, 0.0f}, // vec3 position;    
        {0.0f, 0.0f, 0.0f}, // vec3 La; 
        {1.0f, 0.0f, 1.0f}, // vec3 Ld; 
        {0.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {1.0f, 1.0f, 0.0f}, // vec3 Kd; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ks; 
        0.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 1.0f / glm::sqrt(2.0f), 0.0f, 0.0f };

    std::cout << "Test 5 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Specular light only, cos of 90 deg
void test6()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 1.0f, -1.0f}, // vec3 position;
        {0.0f, 0.0f,  0.0f}, // vec3 La; 
        {0.0f, 0.0f,  0.0f}, // vec3 Ld; 
        {0.1f, 0.2f,  0.3f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.0f, 0.0f, 0.0f}, // vec3 Kd; 
        {0.1f, 0.2f, 0.3f}, // vec3 Ks; 
        1.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.0f, 0.0f, 0.0f };

    std::cout << "Test 6 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Specular light only, cos of 0 deg, shininess is 1.0f
void test7()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 0.0f, 1.0f}, // vec3 position;    <---- the difference
        {0.0f, 0.0f, 0.0f}, // vec3 La; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ld; 
        {0.1f, 0.2f, 0.3f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.0f, 0.0f, 0.0f}, // vec3 Kd; 
        {0.1f, 0.2f, 0.3f}, // vec3 Ks; 
        1.0f                // float shininess;
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.1f * 0.1f, 0.2f * 0.2f, 0.3f * 0.3f };

    std::cout << "Test 7 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Specular light only, cos of 45 deg, shininess is 2.0f 
void test8()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 1.0f, 0.0f };

    Light light = {
        {0.0f, 1.0f, 0.0f}, // vec3 position;  
        {0.0f, 0.0f, 0.0f}, // vec3 La; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ld; 
        {1.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {0.0f, 0.0f, 0.0f}, // vec3 Kd; 
        {1.0f, 0.0f, 0.0f}, // vec3 Ks; 
        2.0f                // float shininess;    <---- the difference
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.382683426f * 0.382683426f, 0.0f, 0.0f };

    std::cout << "Test 8 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// ambient + diffuse + spec 
void test9()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 0.0f, 0.0f}, // vec3 position;  
        {0.1f, 0.2f, 0.3f}, // vec3 La; 
        {0.1f, 0.2f, 0.3f}, // vec3 Ld; 
        {0.1f, 0.2f, 0.3f}  // vec3 Ls;
    };

    Material material = {
        {0.1f, 0.2f, 0.3f}, // vec3 Ka;
        {0.1f, 0.2f, 0.3f}, // vec3 Kd; 
        {0.1f, 0.2f, 0.3f}, // vec3 Ks; 
        0.0f                // float shininess;  
    };

    mat4 view = mat4(1.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 0.01f + 0.01f + 0.01f, 0.04f + 0.04f + 0.04f, 0.09f + 0.09f + 0.09f };

    std::cout << "Test 9 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// view matrix test: diffuse light only, cos of 45 deg, with view transform 0
void test10()
{
    vec3 position = { 0.0f, 0.0f, -1.0f };

    vec3 normal = { 0.0f, 0.0f, 1.0f };

    Light light = {
        {0.0f, 1.0f, 1.0f}, // vec3 position;    
        {0.0f, 0.0f, 0.0f}, // vec3 La; 
        {1.0f, 0.0f, 1.0f}, // vec3 Ld; 
        {0.0f, 0.0f, 0.0f}  // vec3 Ls;
    };

    Material material = {
        {0.0f, 0.0f, 0.0f}, // vec3 Ka;
        {1.0f, 1.0f, 0.0f}, // vec3 Kd; 
        {0.0f, 0.0f, 0.0f}, // vec3 Ks; 
        0.0f                // float shininess;
    };

    mat4 view = mat4(0.0f);

    vec3 actual = BlinnPhong(position, normal, light, material, view);

    vec3 expected = { 1.0f, 0.0f, 0.0f };

    std::cout << "Test 10 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}