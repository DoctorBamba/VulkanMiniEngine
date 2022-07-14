#version 460

#extension GL_KHR_vulkan_glsl                   : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier          : enable

#include "../Materials.glsl"
#include "OceanMap.glsl"

/*****Shader Input*****/

//Attributes
layout(location = 0) in vec4  fragcoord;
layout(location = 1) in vec3  frag_position;
layout(location = 2) in vec2  frag_texcoord;
layout(location = 3) in vec3  frag_normal;
layout(location = 4) in vec3  frag_tangent;
layout(location = 5) in vec3  frag_bitangent;
layout(location = 6) in float frag_clipdepth;

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

layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint datablock_index;
	uint unused0;
	uint unused1;
};

#ifndef LIGHT_BUFFER
#define LIGHT_BUFFER
layout(set = 3, binding = 0) uniform LightBuffer
{
    Light light;
};
#endif

//Push constant
layout(push_constant) uniform PushConstant
{
    uint high_map_location;
};

//Textures

layout(set = 1, binding = 2) uniform sampler2D  gbuffer_depth;
layout(set = 1, binding = 12) uniform sampler2D reflection_0;


/*****Shader output*****/

//The GBuffer...
layout(location = 0) out vec4  out_color;

/*****Shader functions*****/
float WaveHigh(in vec3 point, in float time)
{
    float water_hige = -min(texture(cubemaps[high_map_location], point).r, 0.0f);
    float dis = water_hige * water_hige;
    return 1.0 - 1.0 / (1.0 + water_hige * water_hige);
}

float ConvertDepthToZ(in float depth)
{
    return projection_matrix[2][3] / (projection_matrix[2][2] - depth);
}

/*****Shader Entry*****/
void main()
{
    //The object's transform relevant parameters...
    float planet_radius  = object_transform[0][0];
    vec3 planet_position = object_transform[3].xyz;

    vec3 n = normalize(frag_normal);

    vec3 u, v;
    u = normalize(vec3(-n[1], n[0], 0.0));
    v = normalize(cross(n, u));
    const float epsilon = 0.0001;
    float high =   (OceanMap(n, time));
    float highu =  (OceanMap(n + epsilon * u, time));
    float highv =  (OceanMap(n + epsilon * v, time));

    vec3 normal = normalize(cross(epsilon * planet_radius * u + (highu - high) * n, epsilon * planet_radius * v + (highv - high) * n));

    vec2 texcoord_deviation = vec2(highu - high, highv - high);
    vec2 texcoord = vec2(0.5 * fragcoord.x / fragcoord.w + 0.5, 0.5 * fragcoord.y / fragcoord.w + 0.5);

    vec4 surface_color = vec4(0.2, 0.3, 0.8, 0.0);

    float blocker_distance = ConvertDepthToZ(texture(gbuffer_depth, texcoord).r);
    float water_distance = ConvertDepthToZ(fragcoord.z / fragcoord.w);

    float factor = 1.0f - exp(-(blocker_distance - water_distance));

    out_color = vec4(normal, factor);
}