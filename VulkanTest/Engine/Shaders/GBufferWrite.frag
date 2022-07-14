#version 460
#extension GL_KHR_vulkan_glsl                   : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier          : enable

#include "Materials.glsl"

/*****Shader Inputs*****/

//Attributes
layout(location = 0) in vec4  fragcoord;
layout(location = 1) in vec3  frag_position;
layout(location = 2) in vec2  frag_texcoord;
layout(location = 3) in vec3  frag_normal;
layout(location = 4) in vec3  frag_tangent;
layout(location = 5) in vec3  frag_bitangent;
layout(location = 6) in float frag_clipdepth;

//Buffers
layout(set = 0, binding = 0) buffer DatablocksBuffer
{
    Material materials[];
};

layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint material_index;
	uint unused0;
	uint unused1;
};

/*****Shader Output*****/

//The GBuffer...
layout(location = 0) out vec4  gbuffer_color;           //Color
layout(location = 1) out uvec2 gbuffer_position_u;      //Position, TexcoordU
layout(location = 2) out uvec2 gbuffer_normal_v;        //Normal,   TexcoordV
layout(location = 3) out vec4  gbuffer_mrst;            //Metalic, Roughness, Specular, Transmision
layout(location = 4) out vec4  gbuffer_illumination;    //Illumination

/*****Shader Entrie*****/
void main()
{
    //Clip plane test...
    if(frag_clipdepth < 0.0)
        discard;

    //Materials properties...
    Material material = materials[material_index];
    
    if(material.base_color_texture_loc != Infinite)
        material.base_color = texture(textures[material.base_color_texture_loc], frag_texcoord);

    //Normal...
    vec3 normal;
    if(material_index != Infinite)
        normal = ComputeNormalFromMap(materials[material_index].normals_texture_loc, frag_texcoord, frag_tangent, frag_bitangent, frag_normal, 1.0f);
    else
        normal = frag_normal;

    //Write Buffers..
    gbuffer_color           = vec4(material.base_color.rgb, 1.0f);
    gbuffer_position_u[0]   = packHalf2x16(vec2(frag_position.x, frag_position.y));
    gbuffer_position_u[1]   = packHalf2x16(vec2(frag_position.z, frag_texcoord.x));
    gbuffer_normal_v[0]     = packHalf2x16(vec2(normal.x, normal.y));
    gbuffer_normal_v[1]     = packHalf2x16(vec2(normal.z, normal.y));
    gbuffer_mrst            = vec4(material.metalic, material.roughness, material.specular, material.transmision);
    gbuffer_illumination    = vec4(0.0, 0.0, 0.0, 0.0);
}