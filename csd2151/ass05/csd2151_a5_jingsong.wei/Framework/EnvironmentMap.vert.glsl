R"(
#version 330 core

struct Camera 
{
    vec3 position;    // In world space
};

struct Material 
{
    vec4 color;             
    float reflectionFactor; // The light reflection factor
    float eta;              // The ratio of indices of refraction
};

uniform int Pass; // Pass number

layout(location=0) in vec3 VertexPosition;

uniform mat4 M; // Model transform matrix
uniform mat4 V; // View transform matrix
uniform mat4 P; // Projection transform matrix


// Pass 0

out vec3 Vec;

void pass0()
{
    Vec = VertexPosition; 
    gl_Position = P * V * M * vec4(VertexPosition, 1.0f);
}

// Pass 1

layout(location=1) in vec3 VertexNormal;

uniform Camera camera;
uniform Material material;

out vec3 ReflectDir;
out vec3 RefractDir;

void pass1()
{
    vec3 pos = vec3(M * vec4(VertexPosition, 1.0f));

    mat3 N = mat3(vec3(M[0]), vec3(M[1]), vec3(M[2])); // Normal transform matrix
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