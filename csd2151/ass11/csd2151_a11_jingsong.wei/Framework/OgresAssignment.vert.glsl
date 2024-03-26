R"(
#version 330 core

/*
   This vertex shader for deferred normal map shading.
*/

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

uniform int Pass; // Pass number

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec2 VertexTexCoord;
layout(location=3) in vec4 VertexTangent;

uniform Light light[1];
uniform mat4 M; // Model transform matrix
uniform mat4 V; // View transform matrix
uniform mat4 P; // Projection transform matrix


// Pass 0

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord0;

void pass0()
{
    // Get the normal and tangent in view space
    mat4 MV = V * M;
    mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2])); // Normal transform matrix
    vec3 norm = normalize(N * VertexNormal);
    vec3 tang = normalize(N * vec3(VertexTangent));
    // Compute the binormal
    vec3 binormal = normalize(cross(norm, tang)) * VertexTangent.w;

    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z );

    // Transform light direction and view direction to tangent space
    vec3 pos = vec3(MV * vec4(VertexPosition, 1.0f));
    vec3 lightPositionInView = vec3(V * vec4(light[0].position, 1.0f));
    LightDir = toObjectLocal * (lightPositionInView - pos);
    ViewDir = toObjectLocal * normalize(-pos);
    
    TexCoord0 = VertexTexCoord;
    gl_Position = P * MV * vec4(VertexPosition, 1.0f);
}


// Pass 1

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