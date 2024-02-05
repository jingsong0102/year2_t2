/*!
@file BlinnPhong.frag.glsl
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #4.2
@date 02/04/2024
@brief This is the fragement shader file for app utilizes a 
single-pass mesh rendering process with vertex and fragment shaders 
that implement the Blinn-Phong shading method.
*/
R"(
#version 330 core

struct Material 
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

in vec3 Position;       // In view space
in vec3 Normal;         // In view space

uniform Light light[1];
uniform Material material;
uniform mat4 V;             // View transform matrix
uniform int mode;   //shading mode

layout(location=0) out vec4 FragColor;

vec3 Phong(vec3 position, vec3 normal, Light light, Material material, mat4 view)
{
    // calculate light position in the view space
    vec4 lightPosition = view * vec4(light.position, 1.0f);

    // calculate light direction
    vec3 lightDir = vec3(lightPosition) - position;
    if (length(lightDir) > 0.0f)
        lightDir = normalize(lightDir);
    // calculate view direction
    vec3 viewDir = -position;
    if (length(viewDir) > 0.0f)
        viewDir = normalize(viewDir);

    // calculate ambient
    vec3 ambient = material.Ka * light.La;
    // calculate diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = material.Kd * light.Ld * diff;

    // calculate specular 
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        specular = material.Ks * light.Ls * spec;
    }

    return ambient + diffuse + specular;
}


vec3 BlinnPhong(vec3 position, vec3 normal, Light light, Material material, mat4 view)
{
    // calculate light position in the view space
    vec4 lightPosition = view * vec4(light.position, 1.0f);
    // calculate light direction
    vec3 lightDir = vec3(lightPosition) - position;
    if (length(lightDir) > 0.0f)
        lightDir = normalize(lightDir);

    // calculate view direction
    vec3 viewDir = -position;
    if (length(viewDir) > 0.0f)
        viewDir = normalize(viewDir);
    // calculate diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = material.Kd * light.Ld * diff;
    // calculate specular
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);
        specular = material.Ks * light.Ls * spec;
    }
    // calculate ambient
    vec3 ambient = light.La * material.Ka;

    return ambient + diffuse + specular;
}


void main() 
{
    //blinn-phong shading
    if (mode == 0)
    {
        FragColor = vec4(BlinnPhong(Position, normalize(Normal), light[0], material, V), 1.0f);
    }
    //phong shading
    else if (mode == 1)
    {
        FragColor = vec4(Phong(Position, normalize(Normal), light[0], material, V), 1.0f);
    }
}
)"