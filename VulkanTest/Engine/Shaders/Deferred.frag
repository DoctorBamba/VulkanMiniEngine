#version 460

#extension GL_KHR_vulkan_glsl                   : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier          : enable

#include "Materials.glsl"
#include "RayTracing/RayGenerator.glsl"

/*****Shader Inputs*****/

//Attributes
layout(location = 0) in vec2 texcoords;

//UBuffers
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

#ifndef LIGHT_BUFFER
#define LIGHT_BUFFER
layout(set = 3, binding = 0) uniform LightBuffer
{
    Light light;
};
#endif

//The GBuffer(One descriptor range 1.1)
layout(set = 1, binding = 2) uniform sampler2D  gbuffer_depth;          //Depth
layout(set = 1, binding = 3) uniform sampler2D  gbuffer_color;          //Color
layout(set = 1, binding = 4) uniform usampler2D gbuffer_position_u;     //Position, TexcoordU
layout(set = 1, binding = 5) uniform usampler2D gbuffer_normal_v;       //Normal,   TexcoordV
layout(set = 1, binding = 6) uniform sampler2D  gbuffer_mrst;           //Metalic, Roughness, Specular, Transmision

/*****Shader Output*****/
layout(location = 0) out vec4 out_color;


/*****Shader Entrie*****/
void main()
{
    vec2 pixelcoord = texcoords;

    float depth               = texture(gbuffer_depth, pixelcoord).r;
    vec4  color               = texture(gbuffer_color, pixelcoord);
    uvec4 unpacked_position_u = texture(gbuffer_position_u, pixelcoord);
    uvec4 unpacked_normal_v   = texture(gbuffer_normal_v, pixelcoord);
    vec4  mrst                = texture(gbuffer_mrst, pixelcoord);
    
    vec2 positionz_u = unpackHalf2x16(unpacked_position_u[1]);
    vec2 normalz_v   = unpackHalf2x16(unpacked_normal_v[1]);

    vec3 position = vec3(unpackHalf2x16(unpacked_position_u[0]), positionz_u[0]);
    vec3 normal   = normalize(vec3(unpackHalf2x16(unpacked_normal_v[0]), normalz_v[0]));
    vec2 texcoord = vec2(positionz_u[1], normalz_v[1]);

    out_color = SurfaceIntensityFromLight(light, color, mrst, position, normal, camera_position.xyz, camera_direction.xyz);
    gl_FragDepth = depth;
}