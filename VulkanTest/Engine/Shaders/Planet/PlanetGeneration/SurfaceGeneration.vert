#version 450

layout(location = 0) in vec3 in_position;
layout(location = 0) out vec2 out_texcoord;

void main()
{
	out_texcoord = in_position.xy;
}