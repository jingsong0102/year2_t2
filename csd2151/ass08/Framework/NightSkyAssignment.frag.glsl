R"(
#version 420

uniform int Pass; // Pass number

const vec3 FOG_COLOR = vec3(0.0f, 0.0f, 0.0f);
const float FOG_MAXDIST = 15.0f;
const float FOG_MINDIST = 5.0f;

const int CARTOON_LEVELS = 3;

const int CHECKERBOARD_SIZE = 50;
const float CHECKERBOARD_MIXLEVEL = 0.5f; // Mixing level with shade 
                                          // ex: color = mix(shade, checker, CHECKERBOARD_MIXLEVEL);

const int DISCARD_SCALE = 10;  // How many lines per polygon

struct Light 
{
    vec3 position;      // Position of the light source in the world space
    vec3 L;             // Light intensity (for PBR)
    vec3 La;            // Ambient light intensity
    vec3 Ld;            // Diffuse light intensity
    vec3 Ls;            // Specular light intensity
};

// Material properties
struct Material 
{
    // For PBR
    float rough;            // Roughness
    float metal;            // Metallic (1.0f) or dielectric (0.0f)
    vec3 color;             // Diffuse color for dielectrics, f0 for metallic

    float effect;           // Additional effect (discard, cartoon, etc)
};

layout(location=0) out vec4 FragColor;


// Pass 0

layout(binding=0) uniform samplerCube CubeMapTex;

in vec3 Vec;

void pass0() 
{
    // Access the cube map texture
    vec3 color = texture(CubeMapTex, normalize(Vec)).rgb;

    // Gamma correction
    color = pow(color, vec3(1.0f/2.2f));

    FragColor = vec4(color, 1.0f);
}

// Pass 1

uniform mat4 V;         // View transform matrix

uniform Material material;
uniform Light light[1];

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

vec4 checkerboardTexture(vec2 uv, float size)
{
    uv = uv * size;

    float color = mod(floor(uv.x) + floor(uv.y), 2.0f); // x+y even:black  odd:white

    return vec4(color, color, color, 1.0f);
}

float cartoon(float value)
{
    return floor(value * CARTOON_LEVELS) / CARTOON_LEVELS;
}


// PBR stuff

const float PI = 3.14159265358979323846;

//
// The Microgeometry Normal Distribution Function, based on GGX/Trowbrodge-Reitz, 
// that describes the relative concentration of microfacet normals 
// in the direction H. It has an effect on the size and shape 
// of the specular highlight.
//
// Parameter is cosine of the angle between the normal and H which is the halfway vector of 
// both the light direction and the view direction
//
float ggxDistribution(float nDotH) 
{
    float alpha2 = material.rough * material.rough * material.rough * material.rough;
    float d = (nDotH * nDotH) * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * d * d);
}

//
// The Smith Masking-Shadowing Function describes the probability that microfacets with 
// a given normal are visible from both the light direction and the view direction.
//
// Parameter is cosine of the angle between the normal vector and the view direction.
//
float geomSmith(float nDotL) 
{
    float k = (material.rough + 1.0f) * (material.rough + 1.0f) / 8.0f;
    float denom = nDotL * (1.0f - k) + k;
    return 1.0f / denom;
}

//
// Schlick approximation for Fresnel reflection that defines the probability of light
// reflected from an optically flat surface.
//
// Parameter is cosine of the angle between the light direction vector and 
// the halfway vector of both the light direction and the view direction
//
vec3 schlickFresnel(float lDotH) 
{
    vec3 f0 = vec3(0.04f); // Dielectrics
    if (material.metal == 1.0f)
        f0 = material.color;
    return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5);
}

//
// Bidirectional Reflectance Distribution Function.
// This is the common way to model reflectance based on microfacet theory. 
// This theory was developed to describe reflection from general, non-optically flat surfaces. 
// It models the surface as consisting of small facets that are optically flat (mirrors) and 
// are oriented in various directions. Only those that are oriented correctly to reflect toward 
// the viewer can contribute.
//
// Parameters are the position of a fragment and the surface normal in the view space.
//
vec3 microfacetModel(vec3 position, vec3 n, bool isCartoon) 
{  
    vec3 diffuseBrdf = material.color;

    vec3 lightI = light[0].L;
    vec3 lightPositionInView = (V * vec4(light[0].position, 1.0f)).xyz;

    vec3 l = lightPositionInView - position;
    float dist = length(l);
    l = normalize(l);
    lightI *= 100 / (dist * dist); // Intensity is normalized, so scale up by 100 first

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);

    if (isCartoon)
        nDotH = cartoon(nDotH);

    float lDotH = dot(l, h);

    if (isCartoon)
        lDotH = cartoon(lDotH);

    float nDotL = max(dot(n, l), 0.0f);

    if (isCartoon)
        nDotL = cartoon(nDotL);

    float nDotV = dot(n, v);

    if (isCartoon)
        nDotV = cartoon(nDotV);

    vec3 specBrdf = 0.25f * ggxDistribution(nDotH) * schlickFresnel(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}
void pass1() 
{
    bool isCartoon = (material.effect==2.0f);

    vec4 frontColor;
    vec4 backColor;
    frontColor = vec4(microfacetModel(Position, normalize(Normal), isCartoon), 1.0f);
    backColor = vec4(microfacetModel(Position, normalize(-Normal), false), 1.0f);
    
    vec4 effectColor = vec4(0.0f);

    if (material.effect==0.0f) // No effect
        effectColor = frontColor;
    else if (material.effect==1.0f) // Discard
    {
        vec2 f = fract(TexCoord * DISCARD_SCALE);

        // This does not work on some comps.
        //    if ( all( greaterThan(fract(TexCoord * DISCARD_SCALE), vec2(0.05f, 0.05f))) && 
        //          lessThan(fract(TexCoord * DISCARD_SCALE), vec2(0.95f, 0.95f))) )
        //    if ( length(fract(TexCoord * DISCARD_SCALE)-vec2(0.5f, 0.5f)) < 0.5f ) 

        if (f.x>0.05f && f.y>0.05f && f.x<0.95f && f.y<0.95f && 
            length(f-vec2(0.5f, 0.5f)) < 0.5f)
                 discard;

        if (gl_FrontFacing)
            effectColor = frontColor;
        else
            effectColor = backColor;
    }
    else if (material.effect==2.0f) // Cartoon
        effectColor = frontColor;
    else if (material.effect==3.0f) // Checkerboard texture
        effectColor = mix(frontColor, checkerboardTexture(TexCoord, CHECKERBOARD_SIZE), CHECKERBOARD_MIXLEVEL);
    else
        discard;

    float fogFactor = clamp((FOG_MAXDIST - length(Position)) / (FOG_MAXDIST - FOG_MINDIST), 0.0f, 1.0f);

    vec3 color = mix(FOG_COLOR, effectColor.xyz, fogFactor);

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