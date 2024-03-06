/*!*****************************************************************************
\file NightSkyAssignment.vert.glsl
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 8 (NightSky App)
\date 03/06/2024 (MM/DD/YYYY)
\brief This is the vertex shader file for NightSky App.
*******************************************************************************/
R"(

#version 330 core

/*
   This vertex shader simply converts the position and normal 
   to camera space and passes them to the fragment shader.
   
*/

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 M; // Model transform matrix
uniform mat4 V; // View transform matrix
uniform mat4 P; // Projection transform matrix

//for pass 0
out vec3 Vec;

uniform int Pass;
void pass0()
{
    mat4 MV = V * M; // Model-View transform matrix
    mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2])); // Normal transform matrix 

    vec4 VertexPositionInView = MV * vec4(VertexPosition, 1.0f);
    gl_Position = P * VertexPositionInView; 

    Vec = VertexPosition; 
}

void pass1()
{
    TexCoord = VertexTexCoord;

    mat4 MV = V * M; // Model-View transform matrix
    mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2])); // Normal transform matrix

    Normal = normalize(N * VertexNormal);

    vec4 VertexPositionInView = MV * vec4(VertexPosition, 1.0f);
    Position = VertexPositionInView.xyz;
    gl_Position = P * VertexPositionInView;
}

void main()
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"