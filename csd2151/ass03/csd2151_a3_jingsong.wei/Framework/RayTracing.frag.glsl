/*!
@file RayTracing.frag.glsl
@author Vadim Surov (vsurov@digipen.edu)
@co-author Wei Jingsong (jingsong.wei@digipen.edu)
@course csd2151
@section A
@assignent #2
@date 1/28/2024
@brief The Ray Tracing demo fragment shader.
*/
R"(
#version 330 core

in vec3 TexCoord;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform bool isNormal;

layout(location=0) out vec4 FragColor;

struct Light
{
    vec3 position;
};

struct Ray
{
    vec3 origin;
    vec3 direction; // Normalized
};

struct Sphere
{
    vec3 center;
    float radius;
};

float time_intersect_ray_sphere(Ray ray, Sphere sphere)
{

    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return 0.0f; // No intersection
    }

    // Find the nearest positive root
    float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0f * a);

    if (t1 > 0) return t1;
    if (t2 > 0) return t2;
    return 0.0f; // Both roots are negative, no visible intersection
}
vec3 rayTracing(Ray ray, Sphere sphere)
{
    float t = time_intersect_ray_sphere(ray, sphere);

    vec3 finalColor = vec3(0.0f);

    if (t>0.0f)
    {
        vec3 hitpos = ray.origin + t * ray.direction;
        vec3 n = normalize(hitpos - sphere.center);

        finalColor = abs(vec3(n.z,n.z,n.z));       
    }

    return finalColor;
}
vec3 rayTracing(Ray ray, Sphere sphere, Light light)
{

    float t = time_intersect_ray_sphere(ray, sphere);
    if (t == 0.0f) {
        return vec3(0.0f); // Ray hits the background
    }

    //point of intersection
    vec3 intersection = ray.origin + t * ray.direction;
    vec3 normal = normalize(intersection - sphere.center);
    vec3 light_dir = normalize(light.position - intersection);

    
    float shade = max(dot(normal, light_dir), 0.0f);
    vec3 color = vec3(shade); 

    
    vec3 ambient = vec3(0.1f);
    color = max(color, ambient);

    return color; 
}

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0f- iResolution.xy) / iResolution.y;
    vec3 xyz = vec3(uv, -1.0f);
    vec3 direction = normalize(xyz); 
    Ray ray = Ray(vec3(0.0f), direction);
    Sphere sphere = Sphere(vec3(0.0f, 0.0f, -2.5f), 1.0f);
    Light light = Light(vec3(10.0f, 10.0f, 10.0f));
    if(isNormal)
    FragColor = vec4(rayTracing(ray, sphere), 1.0f);
    else
    FragColor = vec4(rayTracing(ray, sphere, light), 1.0f);
}
)"