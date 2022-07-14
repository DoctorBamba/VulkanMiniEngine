#version 450

layout(location = 0) in vec3  position;

layout(set = 1, binding = 0) uniform LookBuffer
{
    vec4 camera_position;
    vec4 camera_direction;
    mat4 world_matrix;
    mat4 view_matrix;
    mat4 projection_matrix;
};

layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint material_index;
	uint unused0;
	uint unused1;
};

void main()
{
    vec4 absulote_position = vec4(position, 1.0f) * object_transform;
    gl_Position       = projection_matrix * (view_matrix * absulote_position);
}