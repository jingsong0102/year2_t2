#pragma once

#include <vector>
#include <GL\glew.h>

namespace cg
{
    struct TriangleMesh
    {
        std::vector<GLuint>  indices;
        std::vector<GLfloat> points;
        std::vector<GLfloat> normals;
        std::vector<GLfloat> texCoords;
        std::vector<GLfloat> tangents;

        TriangleMesh(const char* fileName = "");
    };
}