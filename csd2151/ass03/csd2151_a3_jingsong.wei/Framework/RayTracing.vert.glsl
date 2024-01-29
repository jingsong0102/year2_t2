/*!
@file RayTracing.vert.glsl
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #2
@date 1/28/2024
@brief The Ray Tracing demo vertex shader.
*/
R"(
#version 330 core

/*
   2D Pass-Through Vertex Shader 
*/

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec3 VertexTexCoord;

out vec3 TexCoord;

void main() 
{
    TexCoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition.x, VertexPosition.y, 0.0f, 1.0f);
}
)"