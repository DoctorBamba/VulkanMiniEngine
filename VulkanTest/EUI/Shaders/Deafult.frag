#version 450

#extension GL_KHR_vulkan_glsl                   : enable
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier          : enable

/*****Shader Inputs*****/

//Attributes
layout(location = 0) in vec2 texcoords;

//Buffer
layout(set = 1, binding = 0) buffer WidgetBuffer
{
	vec3		widgetTransformRow0;
	uint		widgetWidth;
	vec3		widgetTransformRow1;
	uint		widgetHeight;
};

layout(set = 0, binding = 1) uniform sampler2D WidgetMainTexture;

/*****Shader Output*****/

layout(location = 0) out vec4 out_color;

/*****Shader Entry*****/
void main()
{
	float outline_distance_pixels = max((0.5 - abs(texcoords.x - 0.5)) * widgetWidth, (0.5 - abs(texcoords.y - 0.5)) * widgetHeight);
	
	if(outline_distance_pixels < 10.0)
		out_color = vec4(0.0, 0.0, 0.0, 1.0);
	else
		out_color = vec4(1.0, 1.0, 1.0, 1.0);
}