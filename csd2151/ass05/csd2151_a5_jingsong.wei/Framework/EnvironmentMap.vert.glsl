R"(
#version 330 core

struct Material 
{
    vec4 color;             
    float reflectionFactor; // The light reflection factor
    float eta;              // The ratio of indices of refraction
};
uniform Material material;

struct Camera 
{
    vec3 position;    // In world space
};
uniform Camera camera;

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;

uniform int Pass; 
uniform mat4 M; // Model transform matrix
uniform mat4 V; // View transform matrix
uniform mat4 P; // Projection transform matrix

out vec3 TexCoord;
out vec3 ReflectDir;
out vec3 RefractDir;
// Pass 0
void pass0()
{
    TexCoord = VertexPosition; 
    gl_Position = P * V * M * vec4(VertexPosition, 1.0f);
}

// Pass 1
void pass1()
{
    vec3 pos = vec3(M * vec4(VertexPosition, 1.0f));

    mat3 N = mat3(vec3(M[0]), vec3(M[1]), vec3(M[2])); 
    vec3 norm = N * VertexNormal;

    vec3 view = normalize(pos - camera.position);

    ReflectDir = reflect(view, norm);
    RefractDir = refract(view, norm, material.eta);
    
    gl_Position = P * V * M * vec4(VertexPosition, 1.0f);
}

void main()
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"