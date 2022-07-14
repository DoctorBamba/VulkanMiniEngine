#version 450

layout (triangles) in;
layout (line_strip) out;
layout (max_vertices = 2) out;

layout (location = 0) in vec3 position[];
layout (location = 1) in vec2 texcoord[];
layout (location = 2) in vec3 normal[];

layout (location = 0) out vec4 out_fragment;
layout (location = 1) out vec3 out_position;
layout (location = 2) out vec2 out_texcoord;
layout (location = 3) out vec3 out_normal;
layout (location = 4) out vec3 out_tangent;
layout (location = 5) out vec3 out_bitangent;

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

uint secound_inecies[3] = {0, 2, 3};

void main()
{
    out_position        = 0.333 * (position[0] + position[1] + position[2]);
    out_texcoord        = 0.333 * (texcoord[0] * texcoord[1] * texcoord[2]);
    out_normal          = vec3(0, 0, 1);
    out_tangent         = vec3(0, 0, 0);
    out_bitangent       = vec3(0, 0, 0);
    out_fragment        = projection_matrix * (view_matrix * vec4(out_position, 1.0));
    gl_Position         = out_fragment;
    EmitVertex();

    out_position        = 0.333 * (position[0] + position[1] + position[2]) + normal[0] * 0.1;
    out_texcoord        = 0.333 * (texcoord[0] * texcoord[1] * texcoord[2]);
    out_normal          = vec3(0, 0, 1);
    out_tangent         = vec3(0, 0, 0);
    out_bitangent       = vec3(0, 0, 0);
    out_fragment        = projection_matrix * (view_matrix * vec4(out_position, 1.0));
    gl_Position         = out_fragment;
    EmitVertex();


    EndPrimitive();
} 
