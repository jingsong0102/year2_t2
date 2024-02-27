/*!
@file EnvironmentMap.frag.glsl
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #5.2
@date 02/13/2024
@brief This is the fragment shader file for Environment Map assignment.
*/
R"(
#version 330 core

struct Material 
{
    vec4 color;             
    float reflectionFactor; // The light reflection factor
    float eta;              // The ratio of indices of refraction
};
uniform Material material;
uniform int Pass; 

in vec3 ReflectDir;
in vec3 RefractDir;
in vec3 TexCoord;

layout(location=0) out vec4 FragColor;

vec4 checkboardTexture(vec2 uv, float size)
{
     // Scale the UV coordinates
    uv *= size;

    // Calculate the checker pattern
    int checker = int(floor(uv.x) + floor(uv.y)) % 2;

    // Return black or white based on the checker value
    return checker > 0 ? vec4(1.0f, 1.0f, 1.0f, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

vec2 vec2uv(vec3 v)
{
   //if z normal < 0
    if (v.z < 0.0f)
    {
        float f = -1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;
        //front wall
        if (x >= -1.0f && x < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + x) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if z normal > 0
    else if (v.z > 0.0f)
    {
        float f = 1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;
        //front wall
        if (x > -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - x) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if x normal <0
    if (v.x < 0.0f)
    {
        float f = -1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;
        //front wall
        if (z > -1.0f && z <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - z) / 2.0f, (1.0f + y) / 2.0f);
    }
    //if x normal >0
    else if (v.x > 0.0f)
    {
        float f = 1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;
        //front wall
        if (z >= -1.0f && z < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + z) / 2.0f, (1.0f + y) / 2.0f);
    }
    return vec2(0.0f, 0.0f);
}


// Pass 0
void pass0() 
{
    // Access the cube map texture
    vec3 color = checkboardTexture(vec2uv(normalize(TexCoord)), 10.f).rgb;
    // Gamma correction
    color = pow(color, vec3(1.0f/2.2f));

    FragColor = vec4(color, 1.0f);
}
// Pass 1
void pass1() 
{
    // Access the cube map texture
    vec3 reflectColor = checkboardTexture(vec2uv(normalize(ReflectDir)), 10.0f).rgb;
    vec3 refractColor = checkboardTexture(vec2uv(normalize(RefractDir)), 10.0f).rgb;
    
    vec3 color = mix(refractColor, reflectColor, material.reflectionFactor);
    // Gamma correction
    color = pow(color, vec3(1.0f/2.2f));
    
    FragColor = vec4(color, 1.0f);
}


void main()
{
    if      (Pass==0) pass0();
    else if (Pass==1) pass1();
}
)"