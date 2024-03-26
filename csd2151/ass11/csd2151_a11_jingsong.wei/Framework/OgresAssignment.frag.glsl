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



// For pass 0


in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord0;

layout(location=1) out vec3 LightDirData;
layout(location=2) out vec3 ViewDirData;
layout(location=3) out vec3 TexCoordData;

void pass0() 
{
    // Store light and view directions in textures
    LightDirData = LightDir;
    ViewDirData = ViewDir;
    TexCoordData = vec3(TexCoord0, 0.0f);
}


// For pass 1

in vec2 TexCoord1;

layout(binding=0) uniform sampler2D LightDirTex;
layout(binding=1) uniform sampler2D ViewDirTex;
layout(binding=2) uniform sampler2D TexCoordTex;
layout(binding=3) uniform sampler2D DiffusePng;
layout(binding=4) uniform sampler2D NormalMapPng;

uniform Light light[1];
uniform Material material;

layout(location=0) out vec4 FragColor;

// Blinn-Phong illumination with light and view direction vectors given as parameters.
// All vector parameters must be normalized.
vec3 blinnPhong(vec3 normal, vec3 color, vec3 lightDir, vec3 viewDir)
{
  vec3 ambient = light[0].La * material.Ka * color;

  float lDotN = max(dot(lightDir, normal), 0.0f);
  vec3 diffuse = color * light[0].Ld * material.Kd * lDotN;

  vec3 spec = vec3(0.0f);
  if (lDotN > 0.0f)
  {
    vec3 halfway = normalize(viewDir + lightDir);
    spec = light[0].Ls * material.Ks * pow(max(dot(halfway, normal), 0.0f), material.shininess);
  }
  return ambient + diffuse + spec;
}

void pass1() 
{
    // For debugging
    //FragColor = texture(LightDirTex, TexCoord1);
    //FragColor = texture(ViewDirTex, TexCoord1);    
    //FragColor = texture(TexCoordTex, TexCoord1);
    //FragColor = texture(DiffusePng, TexCoord1);
    //FragColor = texture(NormalMapPng, TexCoord1);

    // Lookup the normal from the normal map
    vec2 texcoord = vec2(texture(TexCoordTex, TexCoord1));

    if (all(equal(texcoord, vec2(0.0f, 0.0f))))
        FragColor = vec4(0.5f);
    else
    {
        vec3 norm = vec3(texture(NormalMapPng, texcoord));
        norm.xy = 2.0f * norm.xy - 1.0f;

         // Retrieve information from textures
        vec3 texColor = vec3(texture(DiffusePng, texcoord));
        vec3 lightDir = vec3(texture(LightDirTex, TexCoord1));
        vec3 viewDir = vec3(texture(ViewDirTex, TexCoord1));

        vec3 color = blinnPhong(norm, 
                                texColor, 
                                normalize(lightDir), 
                                normalize(viewDir));
        FragColor = vec4(pow(color, vec3(1.0f/2.2f)), 1.0f);
    }
}

void main() 
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"