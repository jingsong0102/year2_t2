/*!*****************************************************************************
\file unittests.cpp
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: 5.1 (Environment Mapping)
\date 02/05/2024 (MM/DD/YYYY)
\brief This file has definitions of functions for unit tests that are used for
evaluation the code submitted by students on Moodle as VPL.

This code is not intended to be submitted by a student, so any changes
in this file will not be used in the evaluation on the VPL server.
*******************************************************************************/
#include <iostream>
#include <sstream>

#include "unittests_functions.h"

const float EPSILON = 0.001f;
const vec4 BLACK = vec4(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 WHITE = vec4(1.0f, 1.0f, 1.0f, 1.0f);

bool epsilonEqual(vec2 actual, vec2 expected, float EPSILON)
{
    return  glm::epsilonEqual(actual.x, expected.x, EPSILON) &&
        glm::epsilonEqual(actual.y, expected.y, EPSILON);
}

bool epsilonEqual(vec4 actual, vec4 expected, float EPSILON)
{
    return  glm::epsilonEqual(actual.x, expected.x, EPSILON) &&
        glm::epsilonEqual(actual.y, expected.y, EPSILON) &&
        glm::epsilonEqual(actual.z, expected.z, EPSILON) &&
        glm::epsilonEqual(actual.a, expected.a, EPSILON);
}

std::ostream& operator<<(std::ostream& os, vec2 const& v)
{
    os << v.x << "," << v.y;
    return os;
}

std::ostream& operator<<(std::ostream& os, vec4 const& v)
{
    os << v.x << "," << v.y << "," << v.z << "," << v.a;
    return os;
}


// Non evaluated selftest
void test0()
{

}

// Front wall, bottom-left corner
void test1()
{
    vec3 vector = normalize(vec3(-1.0f, -1.0f, -1.0f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.0f, 0.0f);

    std::cout << "Test 1 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Front wall, center
void test2()
{
    vec3 vector = normalize(vec3(0.0f, 0.0f, -1.0f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.5f, 0.5f);

    std::cout << "Test 2 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Right wall, top-left corner
void test3()
{
    vec3 vector = normalize(vec3(1.0f, 1.0f, -1.0f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.0f, 1.0f);

    std::cout << "Test 3 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Back wall, middle-left point
void test4()
{
    vec3 vector = normalize(vec3(1.0f, 0.0f, 1.0f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.0f, 0.5f);

    std::cout << "Test 4 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Left wall, a point
void test5()
{
    vec3 vector = normalize(vec3(-1.0f, -0.5f, -0.5f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.75f, 0.25f);

    std::cout << "Test 5 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Shoot in the air
void test6()
{
    vec3 vector = normalize(vec3(0.0f, 1.0f, 0.0f));

    vec2 actual = vec2uv(vector);
    vec2 expected = vec2(0.0f, 0.0f);

    std::cout << "Test 6 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// First square
void test7()
{
    vec4 actual = checkerboardTexture(vec2(0.0f, 0.0f), 10.0f);
    vec4 expected = BLACK;

    std::cout << "Test 7 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Still the same first square
void test8()
{
    vec4 actual = checkerboardTexture(vec2(0.99f, 0.99f), 10.0f);
    vec4 expected = BLACK;

    std::cout << "Test 8 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Next square located slightly above is different
void test9()
{
    vec4 actual = checkerboardTexture(vec2(0.99f, 1.01f), 10.0f);
    vec4 expected = WHITE;

    std::cout << "Test 9 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}

// Next square on right hand side is different
void test10()
{
    vec4 actual = checkerboardTexture(vec2(1.01f, 0.99f), 10.0f);
    vec4 expected = WHITE;

    std::cout << "Test 10 : ";
    if (epsilonEqual(actual, expected, EPSILON))
        std::cout << "Pass" << std::endl;
    else
        std::cout << "Failed (" << std::endl << actual << ')' << std::endl;
}