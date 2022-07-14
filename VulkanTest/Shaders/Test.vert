#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 0) out vec4 fragColor;


layout(set = 1, binding = 0) uniform LookBuffer
{
    vec4 camera_position;
    vec4 camera_direction;
    mat4 world_matrix;
    mat4 view_matrix;
    mat4 projection_matrix;
};


vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    vec4 absulote_position = vec4(position[0], position[1] * 2, 0.0f, 1.0f);
    gl_Position = projection_matrix * (view_matrix * absulote_position);
    fragColor = color;
}