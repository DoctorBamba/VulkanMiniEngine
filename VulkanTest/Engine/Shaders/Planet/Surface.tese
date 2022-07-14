#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "PlanetGeneration/SurfaceGeneration.glsl"
#include "../Textures.glsl"

#define PLANET_CUBEMAP_DIMANTIONS 512

//Layout specification.
layout (quads, fractional_even_spacing) in;
 
//In parameters.
layout (location = 0) in vec3 cp_position[];
layout (location = 1) in vec2 cp_texcoord[];


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

layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint material_index;
	uint unused0;
	uint unused1;
};

//Out parameters.

layout (location = 0) out vec4  out_fragcoord;
layout (location = 1) out vec3  out_position;
layout (location = 2) out vec2  out_texcoord;
layout (location = 3) out vec3  out_normal;
layout (location = 4) out vec3  out_tangent;
layout (location = 5) out vec3  out_bitangent;
layout (location = 6) out float out_clipdepth;

vec3 TransformCoVector(in vec3 co_vector_, in mat4 transform_)
{
    //Get the scaling of the transformation
    vec3 scaling = vec3(transform_[0][0] * transform_[0][0] + transform_[0][1] * transform_[0][1] + transform_[0][2] * transform_[0][2],
                        transform_[1][0] * transform_[1][0] + transform_[1][1] * transform_[1][1] + transform_[1][2] * transform_[1][2],
                        transform_[2][0] * transform_[2][0] + transform_[2][1] * transform_[2][1] + transform_[2][2] * transform_[2][2]);
    
    return vec3(vec4(co_vector_, 0.0f) * transform_) / scaling;
}



void CompliteOrthonormalBase(in vec3 e1, out vec3 e2, out vec3 e3)
{
    if(e1.z * e1.z < 0.99)
    {
        float l = sqrt(e1.x * e1.x + e1.y * e1.y);
        e2 = vec3(e1.y / l, -e1.x / l, 0.0);
        e3 = vec3(-e1.x * e1.z / l, -e1.y * e1.z / l, l);
    }
    else
    {
        e2 = vec3(0, 1.0, 0);
        e3 = vec3(1.0, 0, 0);
    }
}

float SamplePlanetMap(vec3 point)
{
    return texture(cubemaps[materials[material_index].highmap_texloc], point).r;
}

void main()
{
    //The object's transform relevant parameters...
    float planet_radius  = object_transform[0][0];
    vec3 planet_position = object_transform[3].xyz;

    float delta_x =  (1.0 / gl_TessLevelInner[0]);
    float delta_y =  (1.0 / gl_TessLevelInner[1]);

    //Interpolate atrributes...
    vec3 n        = normalize(mix(mix(cp_position[0], cp_position[3], gl_TessCoord.x), mix(cp_position[1], cp_position[2], gl_TessCoord.x), gl_TessCoord.y));
    vec3 n1       = normalize(mix(mix(cp_position[0], cp_position[3], gl_TessCoord.x + delta_x), mix(cp_position[1], cp_position[2], gl_TessCoord.x + delta_x), gl_TessCoord.y));
    vec3 n2       = normalize(mix(mix(cp_position[0], cp_position[3], gl_TessCoord.x), mix(cp_position[1], cp_position[2], gl_TessCoord.x), gl_TessCoord.y + delta_y));
    vec2 texcoord = normalize(mix(mix(cp_texcoord[0], cp_texcoord[3], gl_TessCoord.x), mix(cp_texcoord[1], cp_texcoord[2], gl_TessCoord.x), gl_TessCoord.y));

    vec3 point0 =  (planet_radius + SamplePlanetMap(n)) * n;
    vec3 point1 =  (planet_radius + SamplePlanetMap(n1)) * n1;
    vec3 point2 =  (planet_radius + SamplePlanetMap(n2)) * n2;
 
    out_position    = planet_position + point0;
    out_texcoord    = gl_TessCoord.xy;
    out_normal      = -normalize(cross(normalize(point1 - point0), normalize(point2 - point0)));
    out_tangent     = vec3(0, 0, 0);
    out_bitangent   = vec3(0, 0, 0);
    out_fragcoord   = projection_matrix * (view_matrix * vec4(out_position, 1.0));
    out_clipdepth   = dot(out_position, clip_plane.xyz) - clip_plane.w;

    gl_Position     = out_fragcoord;
}