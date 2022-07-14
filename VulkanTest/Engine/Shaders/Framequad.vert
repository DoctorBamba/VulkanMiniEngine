#version 450

layout(location = 0) in vec2 position;
layout(location = 0) out vec2 texcoords;

void main()
{
	texcoords = vec2(0.5 * position.x + 0.5, 0.5 * -position.y + 0.5);
	gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);
}