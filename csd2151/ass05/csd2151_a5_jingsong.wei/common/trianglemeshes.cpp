#include <fstream>
#include <sstream>
#include <map>

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "trianglemeshes.h"

namespace cg
{

    struct Vertex
    {
        int pIdx;
        int nIdx;
        int tcIdx;

        Vertex(std::string& vertString)
            : pIdx(-1), nIdx(-1), tcIdx(-1)
        {
            size_t slash1, slash2;
            slash1 = vertString.find("/");
            pIdx = std::stoi(vertString.substr(0, slash1)) - 1;
            if (slash1 != std::string::npos)
            {
                slash2 = vertString.find("/", slash1 + 1);
                if (slash2 > slash1 + 1)
                    tcIdx = std::stoi(vertString.substr(slash1 + 1, slash2 - slash1 - 1)) - 1;
                nIdx = std::stoi(vertString.substr(slash2 + 1)) - 1;
            }
        }
    };

    void generateQuad(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat side = 2.0f);

    void generatePlane(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents);

    void generateCube(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat size = 1.0f);

    void generateTorus(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat outerRadius = 0.7f, GLfloat innerRadius = 0.3f, GLuint nsides = 10, GLuint nrings = 10);

    void generateSphere(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        float rad = 1.0f, GLuint sl = 20, GLuint st = 20);

    void generateSkyBox(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat size = 50.0f);

    void generateTeapot(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        int grid = 10, const glm::mat4& lidTransform = glm::mat4(1.0f));

    void generate(const char* fileName,
        std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        bool genTangents = false);

    TriangleMesh::TriangleMesh(const char* fileName /*= ""*/)
        : indices{ }, points{ }, normals{ }, texCoords{ }, tangents{ }
    {
        if (!strcmp(fileName, "Quad"))
            generateQuad(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "Plane"))
            generatePlane(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "Cube"))
            generateCube(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "Torus"))
            generateTorus(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "Sphere"))
            generateSphere(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "SkyBox"))
            generateSkyBox(indices, points, normals, texCoords, tangents);
        else if (!strcmp(fileName, "Teapot"))
            generateTeapot(indices, points, normals, texCoords, tangents);
        else if (fileName)
            generate(fileName, indices, points, normals, texCoords, tangents, true);
    }

    void generateQuad(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat side /*= 2.0f*/)
    {
        const GLfloat side2 = side / 2.0f;
        points = {
            -side2,   -side2, 0.0f,
            side2,   -side2, 0.0f,
            side2,    side2, 0.0f,
            -side2,    side2, 0.0f
        };

        normals = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        texCoords = {
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
        };

        indices = {
            0,1,2,0,2,3
        };
    }


    void generatePlane(std::vector<GLuint>& indices,
                std::vector<GLfloat>& points,
                std::vector<GLfloat>& normals,
                std::vector<GLfloat>& texCoords,
                std::vector<GLfloat>& tangents)
    {
        const GLfloat side = 4.0f;
        points = {
           -side,  0.0f,   -side,
               0,  0.0f,   -side,
            side,  0.0f,   -side,
           -side,  0.0f,    0.0f,
               0,  0.0f,    0.0f,
            side,  0.0f,    0.0f,
           -side,  0.0f,    side,
               0,  0.0f,    side,
            side,  0.0f,    side };

        normals = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

        texCoords = {
            0.0f, 0.0f,
            0.5f, 0.0f,
            1.0f, 0.0f,
            0.0f, 0.5f,
            0.5f, 0.5f,
            1.0f, 0.5f,
            0.0f, 1.0f,
            0.5f, 1.0f,
            1.0f, 1.0f
        };

        // 6 7 8
        // 3 4 5
        // 0 1 2
        indices = {
            0,1,3,
            3,1,4,
            4,1,2,
            2,5,4,
            3,4,6,
            6,4,7,
            7,4,5,
            5,8,7
        };
    }


    void generateCube(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat side /*= 1.0f*/)
    {
        points = {
            // Front
           -side, -side, side, side, -side, side, side,  side, side,  -side,  side, side,
           // Right
            side, -side, side, side, -side, -side, side,  side, -side, side,  side, side,
            // Back
            -side, -side, -side, -side,  side, -side, side,  side, -side, side, -side, -side,
            // Left
            -side, -side, side, -side,  side, side, -side,  side, -side, -side, -side, -side,
            // Bottom
            -side, -side, side, -side, -side, -side, side, -side, -side, side, -side, side,
            // Top
            -side,  side, side, side,  side, side, side,  side, -side, -side,  side, -side
        };

        normals = {
            // Front
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            // Right
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            // Back
            0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            // Left
            -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            // Bottom
            0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            // Top
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
        };

        texCoords = {
            // Front
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            // Right
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            // Back
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            // Left
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            // Bottom
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            // Top
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
        };

        indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23
        };
    }


    void generateTorus(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat outerRadius /*= 0.7f*/, GLfloat innerRadius /*= 0.3f*/, GLuint nsides /*= 10*/, GLuint nrings /*= 10*/)
    {
        const int nFaces = nsides * nrings;
        const int nVerts = nsides * (nrings + 1);   // One extra ring to duplicate first ring

        points.resize(3 * nVerts);
        normals.resize(3 * nVerts);
        texCoords.resize(2 * nVerts);
        indices.resize(6 * nFaces);

        // Generate the vertex data
        float ringFactor = glm::two_pi<float>() / nrings;
        float sideFactor = glm::two_pi<float>() / nsides;
        int idx = 0, tidx = 0;
        for (GLuint ring = 0; ring <= nrings; ring++)
        {
            float u = ring * ringFactor;
            float cu = cosf(u);
            float su = sinf(u);
            for (GLuint side = 0; side < nsides; side++)
            {
                float v = side * sideFactor;
                float cv = cosf(v);
                float sv = sinf(v);
                float r = (outerRadius + innerRadius * cv);
                points[idx] = r * cu;
                points[idx + 1] = innerRadius * sv;
                points[idx + 2] = r * su; 
                normals[idx] = cv * cu * r;
                normals[idx + 1] = sv * r;
                normals[idx + 2] = cv * su * r; 
                texCoords[tidx] = u / glm::two_pi<float>();
                texCoords[tidx + 1] = v / glm::two_pi<float>();
                tidx += 2;
                // Normalize
                float len = sqrtf(normals[idx] * normals[idx] +
                    normals[idx + 1] * normals[idx + 1] +
                    normals[idx + 2] * normals[idx + 2]);
                normals[idx] /= len;
                normals[idx + 1] /= len;
                normals[idx + 2] /= len;
                idx += 3;
            }
        }

        idx = 0;
        for (GLuint ring = 0; ring < nrings; ring++)
        {
            GLuint ringStart = ring * nsides;
            GLuint nextRingStart = (ring + 1) * nsides;
            for (GLuint side = 0; side < nsides; side++)
            {
                int nextSide = (side + 1) % nsides;
                // The quad
                indices[idx] = (ringStart + side);
                indices[idx + 1] = (nextRingStart + side);
                indices[idx + 2] = (nextRingStart + nextSide);
                indices[idx + 3] = ringStart + side;
                indices[idx + 4] = nextRingStart + nextSide;
                indices[idx + 5] = (ringStart + nextSide);
                idx += 6;
            }
        }
    }

    void generateSphere(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        float radius /*= 1.0f*/, GLuint nSlices /*= 20*/, GLuint nStacks /*= 20*/)
    {
        const int nVerts = (nSlices + 1) * (nStacks + 1);
        const int nFaces = (nSlices * 2 * (nStacks - 1)) * 3;

        points.resize(3 * nVerts);
        normals.resize(3 * nVerts);
        texCoords.resize(2 * nVerts);
        indices.resize(nFaces);

        // Generate positions and normals
        GLfloat theta, phi;
        GLfloat thetaFac = glm::two_pi<float>() / nSlices;
        GLfloat phiFac = glm::pi<float>() / nStacks;
        GLfloat nx, ny, nz, s, t;
        GLuint idx = 0, tIdx = 0;
        for (GLuint i = 0; i <= nSlices; i++) {
            theta = i * thetaFac;
            s = (GLfloat)i / nSlices;
            for (GLuint j = 0; j <= nStacks; j++) {
                phi = j * phiFac;
                t = (GLfloat)j / nStacks;
                nx = sinf(phi) * cosf(theta);
                ny = sinf(phi) * sinf(theta);
                nz = cosf(phi);
                points[idx] = radius * nx; points[idx + 1] = radius * ny; points[idx + 2] = radius * nz;
                normals[idx] = nx; normals[idx + 1] = ny; normals[idx + 2] = nz;
                idx += 3;

                texCoords[tIdx] = s;
                texCoords[tIdx + 1] = t;
                tIdx += 2;
            }
        }

        // Generate the element list
        idx = 0;
        for (GLuint i = 0; i < nSlices; i++) {
            GLuint stackStart = i * (nStacks + 1);
            GLuint nextStackStart = (i + 1) * (nStacks + 1);
            for (GLuint j = 0; j < nStacks; j++)
                if (j == 0) 
                {
                    indices[idx] = stackStart;
                    indices[idx + 1] = stackStart + 1;
                    indices[idx + 2] = nextStackStart + 1;
                    idx += 3;
                }
                else if (j == nStacks - 1) 
                {
                    indices[idx] = stackStart + j;
                    indices[idx + 1] = stackStart + j + 1;
                    indices[idx + 2] = nextStackStart + j;
                    idx += 3;
                }
                else 
                {
                    indices[idx] = stackStart + j;
                    indices[idx + 1] = stackStart + j + 1;
                    indices[idx + 2] = nextStackStart + j + 1;
                    indices[idx + 3] = nextStackStart + j;
                    indices[idx + 4] = stackStart + j;
                    indices[idx + 5] = nextStackStart + j + 1;
                    idx += 6;
                }
        }
    }


    void generateSkyBox(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        GLfloat side /*= 50.0f*/)
    {
        const GLfloat side2 = side / 2.0f;

        points = {
            // Front
           -side2, -side2, side2,
            side2, -side2, side2,
            side2,  side2, side2,
           -side2,  side2, side2,
           // Right
            side2, -side2, side2,
            side2, -side2, -side2,
            side2,  side2, -side2,
            side2,  side2, side2,
            // Back
            -side2, -side2, -side2,
            -side2,  side2, -side2,
             side2,  side2, -side2,
             side2, -side2, -side2,
             // Left
             -side2, -side2, side2,
             -side2,  side2, side2,
             -side2,  side2, -side2,
             -side2, -side2, -side2,
             // Bottom
             -side2, -side2, side2,
             -side2, -side2, -side2,
              side2, -side2, -side2,
              side2, -side2, side2,
              // Top
              -side2,  side2, side2,
               side2,  side2, side2,
               side2,  side2, -side2,
              -side2,  side2, -side2
        };

        // We don't shade a sky box, so normals aren't used.
        normals.resize(points.size(), 0.0f);

        indices = {
            0,2,1,0,3,2,
            4,6,5,4,7,6,
            8,10,9,8,11,10,
            12,14,13,12,15,14,
            16,18,17,16,19,18,
            20,22,21,20,23,22
        };
    }

    /* Copyright (c) Mark J. Kilgard, 1994. */

    /**
    (c) Copyright 1993, Silicon Graphics, Inc.

    ALL RIGHTS RESERVED

    Permission to use, copy, modify, and distribute this software
    for any purpose and without fee is hereby granted, provided
    that the above copyright notice appear in all copies and that
    both the copyright notice and this permission notice appear in
    supporting documentation, and that the name of Silicon
    Graphics, Inc. not be used in advertising or publicity
    pertaining to distribution of the software without specific,
    written prior permission.

    THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
    "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
    OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
    EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
    ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
    INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
    SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
    NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
    OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
    PERFORMANCE OF THIS SOFTWARE.

    US Government Users Restricted Rights

    Use, duplication, or disclosure by the Government is subject to
    restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
    (c)(1)(ii) of the Rights in Technical Data and Computer
    Software clause at DFARS 252.227-7013 and/or in similar or
    successor clauses in the FAR or the DOD or NASA FAR
    Supplement.  Unpublished-- rights reserved under the copyright
    laws of the United States.  Contractor/manufacturer is Silicon
    Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
    94039-7311.

    OpenGL(TM) is a trademark of Silicon Graphics, Inc.
    */

    /* Rim, body, lid, and bottom data must be reflected in x and
       y; handle and spout data across the y axis only.  */

    namespace TeapotData {
        static int patchdata[][16] =
        {
            /* rim */
            {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            /* body */
            {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
            {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
            /* lid */
            {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
            {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
            /* bottom */
            {118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
            /* handle */
            {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
            {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
            /* spout */
            {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
            {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
        };

        static float cpdata[][3] =
        {
            {0.2f, 0.f, 2.7f},
            {0.2f, -0.112f, 2.7f},
            {0.112f, -0.2f, 2.7f},
            {0.f, -0.2f, 2.7f},
            {1.3375f, 0.f, 2.53125f},
            {1.3375f, -0.749f, 2.53125f},
            {0.749f, -1.3375f, 2.53125f},
            {0.f, -1.3375f, 2.53125f},
            {1.4375f, 0.f, 2.53125f},
            {1.4375f, -0.805f, 2.53125f},
            {0.805f, -1.4375f, 2.53125f},
            {0.f, -1.4375f, 2.53125f},
            {1.5f, 0.f, 2.4f},
            {1.5f, -0.84f, 2.4f},
            {0.84f, -1.5f, 2.4f},
            {0.f, -1.5f, 2.4f},
            {1.75f, 0.f, 1.875f},
            {1.75f, -0.98f, 1.875f},
            {0.98f, -1.75f, 1.875f},
            {0.f, -1.75f, 1.875f},
            {2.f, 0.f, 1.35f},
            {2.f, -1.12f, 1.35f},
            {1.12f, -2.f, 1.35f},
            {0.f, -2.f, 1.35f},
            {2.f, 0.f, 0.9f},
            {2.f, -1.12f, 0.9f},
            {1.12f, -2.f, 0.9f},
            {0.f, -2.f, 0.9f},
            {-2.f, 0.f, 0.9f},
            {2.f, 0.f, 0.45f},
            {2.f, -1.12f, 0.45f},
            {1.12f, -2.f, 0.45f},
            {0.f, -2.f, 0.45f},
            {1.5f, 0.f, 0.225f},
            {1.5f, -0.84f, 0.225f},
            {0.84f, -1.5f, 0.225f},
            {0.f, -1.5f, 0.225f},
            {1.5f, 0.f, 0.15f},
            {1.5f, -0.84f, 0.15f},
            {0.84f, -1.5f, 0.15f},
            {0.f, -1.5f, 0.15f},
            {-1.6f, 0.f, 2.025f},
            {-1.6f, -0.3f, 2.025f},
            {-1.5f, -0.3f, 2.25f},
            {-1.5f, 0.f, 2.25f},
            {-2.3f, 0.f, 2.025f},
            {-2.3f, -0.3f, 2.025f},
            {-2.5f, -0.3f, 2.25f},
            {-2.5f, 0.f, 2.25f},
            {-2.7f, 0.f, 2.025f},
            {-2.7f, -0.3f, 2.025f},
            {-3.f, -0.3f, 2.25f},
            {-3.f, 0.f, 2.25f},
            {-2.7f, 0.f, 1.8f},
            {-2.7f, -0.3f, 1.8f},
            {-3.f, -0.3f, 1.8f},
            {-3.f, 0.f, 1.8f},
            {-2.7f, 0.f, 1.575f},
            {-2.7f, -0.3f, 1.575f},
            {-3.f, -0.3f, 1.35f},
            {-3.f, 0.f, 1.35f},
            {-2.5f, 0.f, 1.125f},
            {-2.5f, -0.3f, 1.125f},
            {-2.65f, -0.3f, 0.9375f},
            {-2.65f, 0.f, 0.9375f},
            {-2.f, -0.3f, 0.9f},
            {-1.9f, -0.3f, 0.6f},
            {-1.9f, 0.f, 0.6f},
            {1.7f, 0.f, 1.425f},
            {1.7f, -0.66f, 1.425f},
            {1.7f, -0.66f, 0.6f},
            {1.7f, 0.f, 0.6f},
            {2.6f, 0.f, 1.425f},
            {2.6f, -0.66f, 1.425f},
            {3.1f, -0.66f, 0.825f},
            {3.1f, 0.f, 0.825f},
            {2.3f, 0.f, 2.1f},
            {2.3f, -0.25f, 2.1f},
            {2.4f, -0.25f, 2.025f},
            {2.4f, 0.f, 2.025f},
            {2.7f, 0.f, 2.4f},
            {2.7f, -0.25f, 2.4f},
            {3.3f, -0.25f, 2.4f},
            {3.3f, 0.f, 2.4f},
            {2.8f, 0.f, 2.475f},
            {2.8f, -0.25f, 2.475f},
            {3.525f, -0.25f, 2.49375f},
            {3.525f, 0.f, 2.49375f},
            {2.9f, 0.f, 2.475f},
            {2.9f, -0.15f, 2.475f},
            {3.45f, -0.15f, 2.5125f},
            {3.45f, 0.f, 2.5125f},
            {2.8f, 0.f, 2.4f},
            {2.8f, -0.15f, 2.4f},
            {3.2f, -0.15f, 2.4f},
            {3.2f, 0.f, 2.4f},
            {0.f, 0.f, 3.15f},
            {0.8f, 0.f, 3.15f},
            {0.8f, -0.45f, 3.15f},
            {0.45f, -0.8f, 3.15f},
            {0.f, -0.8f, 3.15f},
            {0.f, 0.f, 2.85f},
            {1.4f, 0.f, 2.4f},
            {1.4f, -0.784f, 2.4f},
            {0.784f, -1.4f, 2.4f},
            {0.f, -1.4f, 2.4f},
            {0.4f, 0.f, 2.55f},
            {0.4f, -0.224f, 2.55f},
            {0.224f, -0.4f, 2.55f},
            {0.f, -0.4f, 2.55f},
            {1.3f, 0.f, 2.55f},
            {1.3f, -0.728f, 2.55f},
            {0.728f, -1.3f, 2.55f},
            {0.f, -1.3f, 2.55f},
            {1.3f, 0.f, 2.4f},
            {1.3f, -0.728f, 2.4f},
            {0.728f, -1.3f, 2.4f},
            {0.f, -1.3f, 2.4f},
            {0.f, 0.f, 0.f},
            {1.425f, -0.798f, 0.f},
            {1.5f, 0.f, 0.075f},
            {1.425f, 0.f, 0.f},
            {0.798f, -1.425f, 0.f},
            {0.f, -1.5f, 0.075f},
            {0.f, -1.425f, 0.f},
            {1.5f, -0.84f, 0.075f},
            {0.84f, -1.5f, 0.075f}
        };
    }

    void generatePatches(std::vector<GLfloat>& p,
        std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc,
        std::vector<GLuint>& el, int grid);
    void buildPatchReflect(int patchNum,
        std::vector<GLfloat>& B, std::vector<GLfloat>& dB,
        std::vector<GLfloat>& v, std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc, std::vector<GLuint>& el,
        int& index, int& elIndex, int& tcIndex, int grid,
        bool reflectX, bool reflectY);
    void buildPatch(glm::vec3 patch[][4],
        std::vector<GLfloat>& B, std::vector<GLfloat>& dB,
        std::vector<GLfloat>& v, std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc, std::vector<GLuint>& el,
        int& index, int& elIndex, int& tcIndex, int grid, glm::mat3 reflect,
        bool invertNormal);
    void getPatch(int patchNum, glm::vec3 patch[][4], bool reverseV);

    void computeBasisFunctions(std::vector<GLfloat>& B, std::vector<GLfloat>& dB, int grid);
    glm::vec3 evaluate(int gridU, int gridV, std::vector<GLfloat>& B, glm::vec3 patch[][4]);
    glm::vec3 evaluateNormal(int gridU, int gridV, std::vector<GLfloat>& B, std::vector<GLfloat>& dB, glm::vec3 patch[][4]);
    void moveLid(int grid, std::vector<GLfloat>& p, const glm::mat4& lidTransform);

    void generateTeapot(std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents,
        int grid /*= 10*/, const glm::mat4& lidTransform /*= glm::mat4(1.0f)*/)
    {
        const int nFaces = grid * grid * 32;
        const int nVerts = 32 * (grid + 1) * (grid + 1);   // One extra ring to duplicate first ring

        points.resize(3 * nVerts);
        normals.resize(3 * nVerts);
        texCoords.resize(2 * nVerts);
        indices.resize(6 * nFaces);

        generatePatches(points, normals, texCoords, indices, grid);

        const glm::mat4 T =
                glm::scale(
                    glm::rotate(glm::mat4(1.0f), glm::pi<float>()/2, glm::vec3(1.0f, 0.0f, 0.0f)),
                    glm::vec3(0.5f, 0.5f, 0.5f));

        for (int i = 0; i < points.size(); i+=3)
        {
            const float x = points[i + 0] * T[0][0] + points[i + 1] * T[0][1] + points[i + 2] * T[0][2];
            const float y = points[i + 0] * T[1][0] + points[i + 1] * T[1][1] + points[i + 2] * T[1][2] - 0.5f;
            const float z = points[i + 0] * T[2][0] + points[i + 1] * T[2][1] + points[i + 2] * T[2][2];
            points[i + 0] = x;
            points[i + 1] = y;
            points[i + 2] = z;
        }

        for (int i = 0; i < normals.size(); i += 3)
        {
            const float x = normals[i + 0] * T[0][0] + normals[i + 1] * T[0][1] + normals[i + 2] * T[0][2];
            const float y = normals[i + 0] * T[1][0] + normals[i + 1] * T[1][1] + normals[i + 2] * T[1][2];
            const float z = normals[i + 0] * T[2][0] + normals[i + 1] * T[2][1] + normals[i + 2] * T[2][2];
            normals[i + 0] = x;
            normals[i + 1] = y;
            normals[i + 2] = z;
        }

        moveLid(grid, points, lidTransform);
    }

    void generatePatches(
        std::vector<GLfloat>& p,
        std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc,
        std::vector<GLuint>& el,
        int grid)
    {
        std::vector<GLfloat> B(4 * (grid + 1));  // Pre-computed Bernstein basis functions
        std::vector<GLfloat> dB(4 * (grid + 1)); // Pre-computed derivitives of basis functions

        int idx = 0, elIndex = 0, tcIndex = 0;

        // Pre-compute the basis functions  (Bernstein polynomials)
        // and their derivatives
        computeBasisFunctions(B, dB, grid);

        // Build each patch
        // The rim
        buildPatchReflect(0, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        // The body
        buildPatchReflect(1, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        buildPatchReflect(2, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        // The lid
        buildPatchReflect(3, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        buildPatchReflect(4, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        // The bottom
        buildPatchReflect(5, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
        // The handle
        buildPatchReflect(6, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
        buildPatchReflect(7, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
        // The spout
        buildPatchReflect(8, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
        buildPatchReflect(9, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    }

    void moveLid(int grid, std::vector<GLfloat>& p, const glm::mat4& lidTransform) {

        int start = 3 * 12 * (grid + 1) * (grid + 1);
        int end = 3 * 20 * (grid + 1) * (grid + 1);

        for (int i = start; i < end; i += 3)
        {
            glm::vec4 vert = glm::vec4(p[i], p[i + 1], p[i + 2], 1.0f);
            vert = lidTransform * vert;
            p[i] = vert.x;
            p[i + 1] = vert.y;
            p[i + 2] = vert.z;
        }
    }

    void buildPatchReflect(int patchNum,
        std::vector<GLfloat>& B, std::vector<GLfloat>& dB,
        std::vector<GLfloat>& v, std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc, std::vector<GLuint>& el,
        int& index, int& elIndex, int& tcIndex, int grid,
        bool reflectX, bool reflectY)
    {
        glm::vec3 patch[4][4];
        glm::vec3 patchRevV[4][4];
        getPatch(patchNum, patch, false);
        getPatch(patchNum, patchRevV, true);

        // Patch without modification
        buildPatch(patch, B, dB, v, n, tc, el,
            index, elIndex, tcIndex, grid, glm::mat3(1.0f), true);

        // Patch reflected in x
        if (reflectX) {
            buildPatch(patchRevV, B, dB, v, n, tc, el,
                index, elIndex, tcIndex, grid, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)), false);
        }

        // Patch reflected in y
        if (reflectY) {
            buildPatch(patchRevV, B, dB, v, n, tc, el,
                index, elIndex, tcIndex, grid, glm::mat3(glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)), false);
        }

        // Patch reflected in x and y
        if (reflectX && reflectY) {
            buildPatch(patch, B, dB, v, n, tc, el,
                index, elIndex, tcIndex, grid, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)), true);
        }
    }

    void buildPatch(glm::vec3 patch[][4],
        std::vector<GLfloat>& B, std::vector<GLfloat>& dB,
        std::vector<GLfloat>& v, std::vector<GLfloat>& n,
        std::vector<GLfloat>& tc, std::vector<GLuint>& el,
        int& index, int& elIndex, int& tcIndex, int grid, glm::mat3 reflect,
        bool invertNormal)
    {
        int startIndex = index / 3;
        float tcFactor = 1.0f / grid;

        for (int i = 0; i <= grid; i++)
        {
            for (int j = 0; j <= grid; j++)
            {
                glm::vec3 pt = reflect * evaluate(i, j, B, patch);
                glm::vec3 norm = reflect * evaluateNormal(i, j, B, dB, patch);
                if (invertNormal)
                    norm = -norm;

                v[index] = pt.x;
                v[index + 1] = pt.y;
                v[index + 2] = pt.z;

                n[index] = norm.x;
                n[index + 1] = norm.y;
                n[index + 2] = norm.z;

                tc[tcIndex] = i * tcFactor;
                tc[tcIndex + 1] = j * tcFactor;

                index += 3;
                tcIndex += 2;
            }
        }

        for (int i = 0; i < grid; i++)
        {
            int iStart = i * (grid + 1) + startIndex;
            int nextiStart = (i + 1) * (grid + 1) + startIndex;
            for (int j = 0; j < grid; j++)
            {
                el[elIndex] = iStart + j;
                el[elIndex + 1] = nextiStart + j + 1;
                el[elIndex + 2] = nextiStart + j;

                el[elIndex + 3] = iStart + j;
                el[elIndex + 4] = iStart + j + 1;
                el[elIndex + 5] = nextiStart + j + 1;

                elIndex += 6;
            }
        }
    }

    void getPatch(int patchNum, glm::vec3 patch[][4], bool reverseV)
    {
        for (int u = 0; u < 4; u++) {          // Loop in u direction
            for (int v = 0; v < 4; v++) {     // Loop in v direction
                if (reverseV) {
                    patch[u][v] = glm::vec3(
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][0],
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][1],
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]][2]
                    );
                }
                else {
                    patch[u][v] = glm::vec3(
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][0],
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][1],
                        TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][2]
                    );
                }
            }
        }
    }

    void computeBasisFunctions(std::vector<GLfloat>& B, std::vector<GLfloat>& dB, int grid) {
        float inc = 1.0f / grid;
        for (int i = 0; i <= grid; i++)
        {
            float t = i * inc;
            float tSqr = t * t;
            float oneMinusT = (1.0f - t);
            float oneMinusT2 = oneMinusT * oneMinusT;

            B[i * 4 + 0] = oneMinusT * oneMinusT2;
            B[i * 4 + 1] = 3.0f * oneMinusT2 * t;
            B[i * 4 + 2] = 3.0f * oneMinusT * tSqr;
            B[i * 4 + 3] = t * tSqr;

            dB[i * 4 + 0] = -3.0f * oneMinusT2;
            dB[i * 4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
            dB[i * 4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
            dB[i * 4 + 3] = 3.0f * tSqr;
        }
    }


    glm::vec3 evaluate(int gridU, int gridV, std::vector<GLfloat>& B, glm::vec3 patch[][4])
    {
        glm::vec3 p(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                p += patch[i][j] * B[gridU * 4 + i] * B[gridV * 4 + j];
            }
        }
        return p;
    }

    glm::vec3 evaluateNormal(int gridU, int gridV, std::vector<GLfloat>& B, std::vector<GLfloat>& dB, glm::vec3 patch[][4])
    {
        glm::vec3 du(0.0f, 0.0f, 0.0f);
        glm::vec3 dv(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                du += patch[i][j] * dB[gridU * 4 + i] * B[gridV * 4 + j];
                dv += patch[i][j] * B[gridU * 4 + i] * dB[gridV * 4 + j];
            }
        }

        glm::vec3 norm = glm::cross(du, dv);
        if (glm::length(norm) != 0.0f) {
            norm = glm::normalize(norm);
        }

        return norm;
    }


    void generate(const char* fileName,
        std::vector<GLuint>& indices,
        std::vector<GLfloat>& points,
        std::vector<GLfloat>& normals,
        std::vector<GLfloat>& texCoords,
        std::vector<GLfloat>& tangents, 
        bool genTangents /*= false*/)
    {
    
        struct MeshData {
            std::vector <glm::vec3> points;
            std::vector <glm::vec3> normals;
            std::vector <glm::vec2> texCoords;
            std::vector <Vertex> faces;
            std::vector <glm::vec4> tangents;
        } meshData;

        // Load data
        std::ifstream objStream(fileName, std::ios::in);

        if (!objStream) 
            throw std::runtime_error(std::string("file ") + fileName + " not found.");

        std::string line, token;
        getline(objStream, line);
        while (!objStream.eof())
        {
            // Remove comment if it exists
            size_t pos = line.find_first_of("#");
            if (pos != std::string::npos)
                line = line.substr(0, pos);

            // Trim string
            const char* whiteSpace = " \t\n\r";
            size_t location;
            location = line.find_first_not_of(whiteSpace);
            line.erase(0, location);
            location = line.find_last_not_of(whiteSpace);
            line.erase(location + 1);

            if (line.length() > 0) {
                std::istringstream lineStream(line);

                lineStream >> token;

                if (token == "v") {
                    float x, y, z;
                    lineStream >> x >> y >> z;
                    glm::vec3 p(x, y, z);
                    meshData.points.push_back(p);
                }
                else if (token == "vt") {
                    // Process texture coordinate
                    float s, t;
                    lineStream >> s >> t;
                    meshData.texCoords.push_back(glm::vec2(s, t));
                }
                else if (token == "vn") {
                    float x, y, z;
                    lineStream >> x >> y >> z;
                    meshData.normals.push_back(glm::vec3(x, y, z));
                }
                else if (token == "f") {
                    std::vector<std::string> parts;
                    while (lineStream.good()) {
                        std::string s;
                        lineStream >> s;
                        parts.push_back(s);
                    }

                    // Triangulate as a triangle fan
                    if (parts.size() > 2) {
                        Vertex firstVert(parts[0]);
                        for (int i = 2; i < parts.size(); i++) {
                            meshData.faces.push_back(firstVert);
                            meshData.faces.push_back(Vertex(parts[i - 1]));
                            meshData.faces.push_back(Vertex(parts[i]));
                        }
                    }
                }
            }
            getline(objStream, line);
        }
        objStream.close();

        // Generate normals if needed
        if (meshData.normals.size() == 0)
        {
            meshData.normals.resize(meshData.points.size());

            for (GLuint i = 0; i < meshData.faces.size(); i += 3)
            {
                const glm::vec3& p1 = meshData.points[meshData.faces[i].pIdx];
                const glm::vec3& p2 = meshData.points[meshData.faces[i + 1].pIdx];
                const glm::vec3& p3 = meshData.points[meshData.faces[i + 2].pIdx];

                glm::vec3 a = p2 - p1;
                glm::vec3 b = p3 - p1;
                glm::vec3 n = glm::normalize(glm::cross(a, b));

                meshData.normals[meshData.faces[i].pIdx] += n;
                meshData.normals[meshData.faces[i + 1].pIdx] += n;
                meshData.normals[meshData.faces[i + 2].pIdx] += n;

                // Set the normal index to be the same as the point index
                meshData.faces[i].nIdx = meshData.faces[i].pIdx;
                meshData.faces[i + 1].nIdx = meshData.faces[i + 1].pIdx;
                meshData.faces[i + 2].nIdx = meshData.faces[i + 2].pIdx;
            }

            for (GLuint i = 0; i < meshData.normals.size(); i++)
                meshData.normals[i] = glm::normalize(meshData.normals[i]);
        }

        // Generate tangents if needed
        if (genTangents)
        {
            std::vector<glm::vec3> tan1Accum(meshData.points.size());
            std::vector<glm::vec3> tan2Accum(meshData.points.size());
            meshData.tangents.resize(meshData.points.size());

            // Compute the tangent std::vector
            for (GLuint i = 0; i < meshData.faces.size(); i += 3)
            {
                const glm::vec3& p1 = meshData.points[meshData.faces[i].pIdx];
                const glm::vec3& p2 = meshData.points[meshData.faces[i + 1].pIdx];
                const glm::vec3& p3 = meshData.points[meshData.faces[i + 2].pIdx];

                const glm::vec2& tc1 = meshData.texCoords[meshData.faces[i].tcIdx];
                const glm::vec2& tc2 = meshData.texCoords[meshData.faces[i + 1].tcIdx];
                const glm::vec2& tc3 = meshData.texCoords[meshData.faces[i + 2].tcIdx];

                glm::vec3 q1 = p2 - p1;
                glm::vec3 q2 = p3 - p1;
                float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
                float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
                float r = 1.0f / (s1 * t2 - s2 * t1);
                glm::vec3 tan1((t2 * q1.x - t1 * q2.x) * r,
                    (t2 * q1.y - t1 * q2.y) * r,
                    (t2 * q1.z - t1 * q2.z) * r);
                glm::vec3 tan2((s1 * q2.x - s2 * q1.x) * r,
                    (s1 * q2.y - s2 * q1.y) * r,
                    (s1 * q2.z - s2 * q1.z) * r);
                tan1Accum[meshData.faces[i].pIdx] += tan1;
                tan1Accum[meshData.faces[i + 1].pIdx] += tan1;
                tan1Accum[meshData.faces[i + 2].pIdx] += tan1;
                tan2Accum[meshData.faces[i].pIdx] += tan2;
                tan2Accum[meshData.faces[i + 1].pIdx] += tan2;
                tan2Accum[meshData.faces[i + 2].pIdx] += tan2;
            }

            for (GLuint i = 0; i < meshData.points.size(); ++i)
            {
                const glm::vec3& n = meshData.normals[i];
                glm::vec3& t1 = tan1Accum[i];
                glm::vec3& t2 = tan2Accum[i];

                // Gram-Schmidt orthogonalize
                meshData.tangents[i] = glm::vec4(glm::normalize(t1 - (glm::dot(n, t1) * n)), 0.0f);
                // Store handedness in w
                meshData.tangents[i].w = (glm::dot(glm::cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
            }
        }

        // Convert to GL format
        std::map<std::string, GLuint> vertexMap;
        for (auto& vert : meshData.faces)
        {
            std::string vertStr = std::to_string(vert.pIdx) + "/" +
                std::to_string(vert.tcIdx) + "/" +
                std::to_string(vert.nIdx);
            auto it = vertexMap.find(vertStr);
            if (it == vertexMap.end()) {
                auto vIdx = points.size() / 3;

                auto& pt = meshData.points[vert.pIdx];
                points.push_back(pt.x);
                points.push_back(pt.y);
                points.push_back(pt.z);

                auto& n = meshData.normals[vert.nIdx];
                normals.push_back(n.x);
                normals.push_back(n.y);
                normals.push_back(n.z);

                if (!meshData.texCoords.empty()) {
                    auto& tc = meshData.texCoords[vert.tcIdx];
                    texCoords.push_back(tc.x);
                    texCoords.push_back(tc.y);
                }

                if (!meshData.tangents.empty()) {
                    // We use the point index for tangents
                    auto& tang = meshData.tangents[vert.pIdx];
                    tangents.push_back(tang.x);
                    tangents.push_back(tang.y);
                    tangents.push_back(tang.z);
                    tangents.push_back(tang.w);
                }

                indices.push_back((GLuint)vIdx);
                vertexMap[vertStr] = (GLuint)vIdx;
            }
            else
                indices.push_back(it->second);
        }

    }

}