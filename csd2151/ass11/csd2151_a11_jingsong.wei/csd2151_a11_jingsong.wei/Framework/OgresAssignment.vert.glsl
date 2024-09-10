/*!*****************************************************************************
\file OgresAssignment.vert.glsl
\author Vadim Surov (vsurov\@digipen.edu)
\co-author WEI JINGSONG (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 11
\date 03/31/2024 (MM/DD/YYYY)
\brief This file has definitions of the vertex shader for OgresAssignment.
*******************************************************************************/
R"(
#version 330 core

uniform int Pass; // Pass number

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec2 VertexTexCoord;
layout(location=3) in vec4 VertexTangent;

uniform Light light[1];
uniform mat4 M;     // Model transform matrix
uniform mat4 V;     // View transform matrix
uniform mat4 P;     // Projection transform matrix

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

void pass0()
{
    mat4 MV = V * M;
    mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2])); // Normal transform matrix

    // Transform normal and tangent to view space and compute the binormal
    vec3 norm = normalize(N * VertexNormal);
    vec3 tang = normalize(N * vec3(VertexTangent));
    vec3 binormal = normalize(cross(norm, tang)) * VertexTangent.w;

    // Matrix for transformation to the tangent space
    mat3 toTangentSpace = mat3(  tang.x, binormal.x, norm.x,
                                tang.y, binormal.y, norm.y,
                                tang.z, binormal.z, norm.z  );

    // Transform light direction and view direction to the tangent space
    vec3 vertexPositionInView = vec3(MV * vec4(VertexPosition, 1.0f));
    vec3 lightPositionInView = vec3(V * vec4(light[0].position, 1.0f));
    LightDir = toTangentSpace * (lightPositionInView - vertexPositionInView);
    ViewDir = toTangentSpace * normalize(-vertexPositionInView);
    TexCoord = VertexTexCoord;
    gl_Position = P * MV * vec4(VertexPosition, 1.0f);
}

out vec2 TexCoord1;

void pass1()
{
    TexCoord1 = VertexTexCoord; 
    gl_Position = P * V * M * vec4(VertexPosition, 1.0f);
}

void main()
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"