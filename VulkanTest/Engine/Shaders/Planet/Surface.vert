#version 450

layout(location = 0) in vec3  position;
layout(location = 1) in vec2  texcoord;
layout(location = 2) in vec3  normal;
layout(location = 3) in vec3  tangent;
layout(location = 4) in vec3  bitangent;

layout(location = 0) out vec3  out_position;
layout(location = 1) out vec2  out_texcoord;


void main()
{
    out_position    = position;
    out_texcoord    = texcoord;
}