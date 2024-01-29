/*!
@file ShaderToy.frag.glsl
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #2
@date 1/21/2024
@brief A ShaerToy alike assignment fragment shader.
*/
R"(
#version 330 core

//
// Inspired by https://www.shadertoy.com/view/mtyGWy
// All credits to kishimisu: https://www.shadertoy.com/user/kishimisu
// Explanations: https://youtu.be/f4s1h2YETNY
//

in vec3 TexCoord;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)

layout(location=0) out vec4 FragColor;

//
// Cosine based palette. More info https://iquilezles.org/articles/palettes/
//
vec3 palette(float t) 
{
    vec3 a = vec3(0.5f, 0.5f, 0.5f);
    vec3 b = vec3(0.5f, 0.5f, 0.5f);
    vec3 c = vec3(1.0f, 1.0f, 1.0f);
    vec3 d = vec3(0.263f, 0.416f, 0.557f);

    return a + b * cos(6.28318f * (c * t + d));
}

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0f - iResolution.xy) / iResolution.y;
    float luv = length(uv);
    float t = exp(-luv) * 8;

    vec3 FinalColor = vec3(0.0f);    
    for (int i = 0; i<4; ++i) 
    {
        uv = fract(uv * 1.5f) - 0.5f;
        float d = pow(1 / (100 * abs(sin(length(uv) * t + iTime) / 8)), 1.2f);
        vec3 col = palette(luv + i * 0.4f + iTime * 0.4f);

        FinalColor += col * d;
    }
        
    FragColor = vec4(FinalColor, 1.0f);
}
)"