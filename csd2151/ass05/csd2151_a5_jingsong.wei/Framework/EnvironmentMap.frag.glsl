R"(
#version 420

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
    uv = uv * size;

    vec2 color = mod(floor(vec2(uv.x, uv.x)) + floor(vec2(uv.y, uv.y)), 2.0f); // x+y even:black  odd:white

    return vec4(color.x, color.x, color.x, 1.0f);
}

vec2 vec2uv(vec3 v)
{
    if (v.z < 0.0f)
    {
        float f = -1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;

        if (x >= -1.0f && x < 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f + x) / 2.0f, (1.0f + y) / 2.0f);
    }
    else if (v.z > 0.0f)
    {
        float f = 1.0f / v.z;
        float x = v.x * f;
        float y = v.y * f;

        if (x > -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - x) / 2.0f, (1.0f + y) / 2.0f);
    }

    if (v.x < 0.0f)
    {
        float f = -1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;

        if (z > -1.0f && z <= 1.0f && y >= -1.0f && y <= 1.0f)
            return vec2((1.0f - z) / 2.0f, (1.0f + y) / 2.0f);
    }
    else if (v.x > 0.0f)
    {
        float f = 1.0f / v.x;
        float z = v.z * f;
        float y = v.y * f;

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