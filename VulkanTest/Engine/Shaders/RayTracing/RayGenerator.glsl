#ifndef LOOK_BUFFER
#define LOOK_BUFFER
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
#endif

void CreateRay(in vec2 uv, out vec3 ray_start, out vec3 ray_direction)
{
    const float near = 1.0;
    const float far  = 1000.0;
    float tan_half_alpha = 1.0 / projection_matrix[0][0];
    float tan_half_beta  = 1.0 / projection_matrix[1][1];
   
    vec3 right      = vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
    vec3 down       = vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
    vec3 direction  = vec3(view_matrix[0][2], view_matrix[1][2], view_matrix[2][2]);
    
    ray_start       = camera_position.xyz + near * (direction + tan_half_alpha * uv.x * right + tan_half_beta * uv.y * down);
    vec3 ray_end    = camera_position.xyz + far * (direction + tan_half_alpha * uv.x * right + tan_half_beta * uv.y * down);
    

    ray_direction   = normalize(ray_end - ray_start);
}