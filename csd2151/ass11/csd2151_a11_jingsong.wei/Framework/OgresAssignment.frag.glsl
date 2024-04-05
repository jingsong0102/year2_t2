/*!*****************************************************************************
\file OgresAssignment.vert.glsl
\author Vadim Surov (vsurov\@digipen.edu)
\co-author WEI JINGSONG (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 11
\date 03/31/2024 (MM/DD/YYYY)
\brief This file has definitions of the fragment shader for OgresAssignment.
*******************************************************************************/
R"(
#version 420

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

struct Material 
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};

uniform int Pass;   // Pass number

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

uniform Light light[1];
uniform Material material;

layout(location=1) out vec3 LightDirData;
layout(location=2) out vec3 ViewDirData;
layout(location=3) out vec2 TexCoordData;

void pass0() 
{
    // Store light view and texturecoordi
    LightDirData = LightDir;
    ViewDirData = ViewDir;
    TexCoordData = TexCoord;
}

in vec2 TexCoord1;

layout(binding=0) uniform sampler2D LightTex;
layout(binding=1) uniform sampler2D ViewTex;
layout(binding=2) uniform sampler2D TexTex;
layout(binding=3) uniform sampler2D ColorTex;
layout(binding=4) uniform sampler2D NormalMapTex;

layout(location=0) out vec4 FragColor;

// Blinn-Phong illumination with light and view direction vectors given as parameters.
// All vector parameters must be normalized.
vec3 blinnPhong(vec3 normal, vec3 color, vec3 lightDir, vec3 viewDir)
{
    //calculate diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = color * diff * light[0].Ld * material.Kd;

    //calculate specular
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess); 
        specular = material.Ks * light[0].Ls * spec; 
    }
    // calculate ambient
    vec3 ambient = light[0].La * material.Ka * color; // Assuming a low ambient factor

   return ambient + diffuse + specular;
}

void pass1() 
{
    // Retrieve information from textures
    vec3 LightDir = vec3(texture(LightTex, TexCoord1));
    vec3 ViewDir = vec3(texture(ViewTex, TexCoord1));
    vec2 Texcoord = vec2(texture(TexTex, TexCoord1));

    if (Texcoord == vec2(0.0f, 0.0f)) {
        FragColor = vec4(0.0f); // Set to fully transparent or to a default color
        return; // Skip further processing
    }
    // Lookup the normal from the normal map texture
    vec3 normal = vec3(texture(NormalMapTex, Texcoord));
    normal.xy = 2.0f * normal.xy - 1.0f;

    // Calculate the illumination
    vec3 color = blinnPhong(normal, 
                            vec3(texture(ColorTex, Texcoord)), 
                            normalize(LightDir), 
                            normalize(ViewDir));
    // Set with the gamma correction
    FragColor = vec4(pow(color, vec3(1.0f/2.2f)), 1.0f);
}

void main() 
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"