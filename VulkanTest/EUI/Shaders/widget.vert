#version 450

/*********Shader Input**********/

//Attributes
layout(location = 0) in vec3 position;
layout(location = 0) out vec2 texcoords;

//Buffer
layout(set = 0, binding = 0) buffer RenderinBuffer
{
	vec2 viewPosition;
	uint viewWidth;
	uint viewHeight;
};

layout(set = 1, binding = 0) buffer WidgetBuffer
{
	vec3		widgetTransformRow0;
	uint		widgetWidth;
	vec3		widgetTransformRow1;
	uint		widgetHeight;
};


void main()
{
	vec3 local_position = vec3(0.5 * position.x + 0.5, 0.5 * -position.y + 0.5, 1.0f);
	vec2 screen_position = vec2(dot(widgetTransformRow0, local_position), dot(widgetTransformRow1, local_position));
	vec2 viewport_position = 2.0f * ((screen_position - viewPosition) / vec2(viewWidth, viewHeight)) - vec2(1.0f, 1.0f);

	gl_Position = vec4(viewport_position, 0.0f, 1.0f);
	texcoords = local_position.xy;
}