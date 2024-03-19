/*!*****************************************************************************
\file FiltersAssignment.frag.glsl
\author Vadim Surov (vsurov\@digipen.edu)
\co-author Wei Jingsong (jingsong.wei@digipen.edu)
\par Course: CSD2151
\par Assignment: 9 (FiltersAssignment App)
\date 03/17/2024 (MM/DD/YYYY)
\brief This is the fragment shader file for FiltersAssignment App.
*******************************************************************************/

R"(

#version 420

uniform int Pass;       // Pass number

layout(location=0) out vec4 FragColor;

// Pass 0

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
uniform mat4 V;         // View transform matrix
uniform int mode = 4;

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

// Render the scene to a texture using the Blinn-Phong shading
void pass0() 
{
    FragColor = vec4(BlinnPhong(Position, normalize(Normal), light[0], material, V), 1.0f);
}

layout(binding=0) uniform sampler2D Texture;

// Precomputed normalized Gaussian weights. 
// We need only half of all weights because they are symmetric about zero.
float Weight[5] = { 0.158435f, 0.148836f, 0.123389f, 0.0902733f, 0.0582848f };

// Applies the weighted vertical sum of the Gaussian blur filter 
// to store the result in yet another texture. 
void pass1() 
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(Texture, pix, 0);

    if (mode==2 || mode==4) 
    {
        sum *= Weight[0];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0,-4)) * Weight[4];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0,-3)) * Weight[3];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0,-2)) * Weight[2];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0,-1)) * Weight[1];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0, 1)) * Weight[1];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0, 2)) * Weight[2];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0, 3)) * Weight[3];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(0, 4)) * Weight[4];
    }
    FragColor = sum;
}

// Same as pass1 but now applies the weighted horizontal sum of the Gaussian filter.
void pass2() 
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(Texture, pix, 0);
        
    if (mode==2 || mode==4)
    {
        sum *= Weight[0];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(-4,0)) * Weight[4];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(-3,0)) * Weight[3];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(-2,0)) * Weight[2];
        sum += texelFetchOffset(Texture, pix, 0, ivec2(-1,0)) * Weight[1];
        sum += texelFetchOffset(Texture, pix, 0, ivec2( 1,0)) * Weight[1];
        sum += texelFetchOffset(Texture, pix, 0, ivec2( 2,0)) * Weight[2];
        sum += texelFetchOffset(Texture, pix, 0, ivec2( 3,0)) * Weight[3];
        sum += texelFetchOffset(Texture, pix, 0, ivec2( 4,0)) * Weight[4];
    }
    FragColor = sum;
}

// Parameters
uniform float EdgeThreshold;

float luminance(vec3 color) 
{
    return dot(vec3(0.2126f, 0.7152f, 0.0722f), color);
}

void pass3() 
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    if (mode==3 || mode==4)
    {
        float s00 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1, 1)).rgb);
        float s10 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1, 0)).rgb);
        float s20 = luminance(texelFetchOffset(Texture, pix, 0, ivec2(-1,-1)).rgb);
        float s01 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 0, 1)).rgb);
        float s21 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 0,-1)).rgb);
        float s02 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1, 1)).rgb);
        float s12 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1, 0)).rgb);
        float s22 = luminance(texelFetchOffset(Texture, pix, 0, ivec2( 1,-1)).rgb);

        float sx = s00 + 2*s10 + s20 - (s02 + 2*s12 + s22);
        float sy = s00 + 2*s01 + s02 - (s20 + 2*s21 + s22);

        float g = sx*sx + sy*sy;

        FragColor = (g > EdgeThreshold) ? vec4(1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
     }
     else 
        FragColor = texelFetchOffset(Texture, pix, 0, ivec2(0, 0));
}

void main() 
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
    else if (Pass==2) pass2();
    else if (Pass==3) pass3();
}
)"