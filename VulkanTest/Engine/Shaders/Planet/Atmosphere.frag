#version 460
#extension GL_KHR_vulkan_glsl : enable

#include "../RayTracing/RayGenerator.glsl"

/*****Shader inputs*****/

//Attributes
layout(location = 0) in vec2 texcoords;

//Buffers

#ifndef LOOK_BUFFER
#define LOOK_BUFFER
layout(set = 1, binding = 0) uniform LookBuffer
{
    vec4  camera_position;
    vec4  camera_direction;
    mat4  view_matrix;
    mat4  projection_matrix;
    vec4  clip_plane;
	float time;
	float lookbuffer_unused0;
	float lookbuffer_unused1;
	float lookbuffer_unused2;
};
#endif

layout(set = 1, binding = 1) uniform PostcallInput
{
    vec3  planet_center;
    float planet_radius;

    vec3  atmosphere_rayleigh_coeff;
    float atmosphere_radius;
    vec3  atmosphere_mie_coeff;
    float atmosphere_mie_scatt_coeff;

    vec3  sun_direction;
    float sun_intensity;
    vec4  sun_color;
};

//Textures
layout(set = 1, binding = 9) uniform sampler2D   image_depth; //Depth
layout(set = 1, binding = 11) uniform sampler2D  image_color; //Color


/*****Shader output*****/
layout(location = 0) out vec4 out_color;


/*****Functions*****/

const float I = 20.0; // Sun intensity
float atmosphere_height = atmosphere_radius - planet_radius;

float ConvertDepthToZ(in float depth)
{
    return projection_matrix[2][3] / (projection_matrix[2][2] - depth);
}

float GreenhouseFunction(float h)
{
    h = h * 0.25;
    return 1.4 / (max(h - 1.0, 0.0) + 1.0) + 0.4;
}

vec2 DensitiesRM(vec3 sample_point)
{ 
    float h0 = atmosphere_height * 0.25;
    float h  = length(sample_point - planet_center) - planet_radius;
    float hR = max(0.0, h - h0) / (h0 + h0);
    float hM = max(0.0, h - h0) / h0;
    return vec2(4.0 * exp(-hR), 0.8 * exp(-hM));
}

// Basically a ray-sphere intersection. Find distance to where rays escapes a sphere with given radius.
// Used to calculate length at which ray escapes atmosphere
float EscapeAtmosphere(vec3 ray_start, vec3 ray_direction)
{
    vec3 v = ray_start - planet_center;
    float b = dot(v, ray_direction);
   
    float det = b * b - dot(v, v) + atmosphere_radius * atmosphere_radius;

    if (det < 0.0)
        return -1.0;
    det = sqrt(det);
    
    float t1 = -b - det;
    float t2 = -b + det;
    return (t1 >= 0.) ? t1 : t2;
}

// Calculate density integral for optical depth for ray starting at point `p` in direction `d` for length `L`
// Perform `steps` steps of integration
// Returns vec2(depth_int_rayleigh, depth_int_mie)
vec2 ScatterDepthInt(vec3 ray_start, vec3 ray_direction, float depth, float steps)
{
    if(depth < 0.0f)
        return vec2(0.0, 0.0);

    vec2 depthRMs = vec2(0.0);

    float ds = depth / steps;
    ray_direction *= ds;
    
    //Go from point P to A
    for (uint i = 0 ; i < steps ; i ++)
    {
        depthRMs += DensitiesRM(ray_start);
        ray_start += ray_direction;
    }

    return depthRMs * ds;
}


// Global variables, needed for size
vec2 optic_depth;
vec3 I_R, I_M;

// Calculate in-scattering for ray starting at point `o` in direction `d` for length `L`
// Perform `steps` steps of integration
void scatterIn(vec3 ray_start, vec3 ray_direction, float depth, float steps) {

    // Set L to be step distance and pre-multiply d with it
    float ds = depth / steps;
    ray_direction *= ds;

    // Go from point O to B
    for (uint i = 0 ; i < steps ; i ++)
    {
        // Calculate densities
        vec2 dRM = DensitiesRM(ray_start) * ds;

        // Accumulate T(P_i -> O) with the new P_i
        optic_depth += dRM;

        // Calculate sum of optical depths. totalDepthRM is T(P_i -> O)
        // scatterDepthInt calculates integral part for T(A -> P_i)
        // So depthRMSum becomes sum of both optical depths
        vec2 depthRMsum = optic_depth + ScatterDepthInt(ray_start, sun_direction, EscapeAtmosphere(ray_start, sun_direction), 8.0);

        // Calculate e^(T(A -> P_i) + T(P_i -> O)
        vec3 A = exp((-atmosphere_rayleigh_coeff * depthRMsum.x - atmosphere_mie_coeff * depthRMsum.y));

        // Accumulate I_R and I_M
        I_R += sun_color.rgb * A * dRM.x;
        I_M += sun_color.rgb * A * dRM.y;

        ray_start += ray_direction;
    }
}

// Final scattering function
// O = o -- starting point
// B = o + d * L -- end point
// Lo -- end point color to calculate extinction for
vec3 scatter(vec3 ray_start, vec3 ray_direction, float depth, vec3 blocker_lum) {

    // Zero T(P -> O) accumulator
    optic_depth = vec2(0.0, 0.0);

    // Zero I_M and I_R
    I_R = vec3(0.0, 0.0, 0.0);
    I_M = vec3(0.0, 0.0, 0.0);

    // Compute T(P -> O) and I_M and I_R
    scatterIn(ray_start, ray_direction, depth, 16);

    // mu = cos(alpha)
    float mu = dot(ray_direction, sun_direction);

    //Greenhouse amplification(Not base on mathematic, just look good)...
    float dis = (length(ray_start - planet_center) - planet_radius) / (atmosphere_radius - planet_radius);
    float intensity = I * 1.5 / (1.5 + min(dis * dis, 2.0f));
    // Calculate Lo extinction
    return blocker_lum * exp(-atmosphere_rayleigh_coeff * optic_depth.x - vec3(atmosphere_mie_scatt_coeff)  * optic_depth.y) +
                                        intensity * (1.0 + mu * mu) * (
                                        I_R * atmosphere_rayleigh_coeff * 0.0597 +
                                        I_M * atmosphere_mie_coeff * 0.0196 / pow(1.58 - 1.52 * mu, 1.5));
}

/*****Shader Entry*****/
void main()
{
    float depth = texture(image_depth, texcoords).r;
    vec4  color = texture(image_color, texcoords);

    vec3 ray_start, ray_direction;
    CreateRay(vec2(2.0 * texcoords.x - 1.0, 2.0 * texcoords.y - 1.0), ray_start, ray_direction);

    out_color = vec4(scatter(ray_start, ray_direction, min(ConvertDepthToZ(depth), 200.0), color.rgb), 1.0);
}
