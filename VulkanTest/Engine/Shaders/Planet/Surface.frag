#version 460

#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "../Textures.glsl"

layout(location = 0) in vec4  fragcoord;
layout(location = 1) in vec3  frag_position;
layout(location = 2) in vec2  frag_texcoord;
layout(location = 3) in vec3  frag_normal;
layout(location = 4) in vec3  frag_tangent;
layout(location = 5) in vec3  frag_bitangent;
layout(location = 6) in float frag_clipdepth;

//The GBuffer...
layout(location = 0) out vec4  gbuffer_color;           //Color
layout(location = 1) out uvec2 gbuffer_position_u;      //Position, TexcoordU
layout(location = 2) out uvec2 gbuffer_normal_v;        //Normal,   TexcoordV
layout(location = 3) out vec4  gbuffer_mrst;            //Metalic, Roughness, Specular, Transmision
layout(location = 4) out vec4  gbuffer_illumination;    //Illumination

struct MaterialProperties
{
    uint highmap_texloc;
    uint rock_color_texloc;
    uint rock_normal_texloc;
    uint ground_color_texloc;
    uint material_padding[60]; // Compile to 256 byte
};

layout(set = 0, binding = 0) buffer MaterialBuffer
{
    MaterialProperties materials[];
};


layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint material_index;
	uint unused0;
	uint unused1;
};

#define MATERIAL materials[material_index]

void main()
{
    //Clip plane test...
    if(frag_clipdepth < 0.0)
        discard;


    //The object's transform relevant parameters...
    float planet_radius  = object_transform[0][0];
    vec3 planet_position = object_transform[3].xyz;

    vec4 color = vec4(0.0, 0.0f, 0.0, 0.0);
    float tangent_factor = dot(frag_normal, (frag_position - planet_position) / planet_radius);
    
    color = (1.0f - tangent_factor) * texture(textures[MATERIAL.rock_color_texloc], frag_texcoord * 2.0)
          + tangent_factor * texture(textures[MATERIAL.ground_color_texloc], frag_texcoord * 2.0);

    //Write Buffers..

    gbuffer_color           = tangent_factor * color;
    gbuffer_position_u[0]   = packHalf2x16(vec2(frag_position.x, frag_position.y));
    gbuffer_position_u[1]   = packHalf2x16(vec2(frag_position.z, frag_texcoord.x));
    gbuffer_normal_v[0]     = packHalf2x16(vec2(frag_normal.x, frag_normal.y));
    gbuffer_normal_v[1]     = packHalf2x16(vec2(frag_normal.z, frag_normal.y));
    gbuffer_mrst            = vec4(0.5, 0.9, 0.0, 0.0);
    gbuffer_illumination    = vec4(0.0, 0.0, 0.0, 0.0);
}