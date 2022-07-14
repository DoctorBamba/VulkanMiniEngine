#version 450
#extension GL_EXT_multiview : enable

#include "SurfaceGeneration.glsl"

layout(location = 0) in vec2 in_texcoord;
layout(location = 0) out float map_output;

const vec3 u[6] = {vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0)};
const vec3 v[6] = {vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0)};
const vec3 w[6] = {vec3(1.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, -1.0)};

void main()
{
	vec3 sphere_point = normalize(w[gl_Layer] + in_texcoord.x * u[gl_Layer] - in_texcoord.y * v[gl_Layer]);
	
	map_output = PlanetMap(sphere_point);
}