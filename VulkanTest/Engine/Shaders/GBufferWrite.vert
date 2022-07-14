#version 450
#define Infinite 0xffffffff

layout(location = 0) in vec3  position;
layout(location = 1) in vec2  texcoord;
layout(location = 2) in vec3  normal;
layout(location = 3) in vec3  tangent;
layout(location = 4) in vec3  bitangent;
layout(location = 5) in uvec4 bones_ids;
layout(location = 6) in vec4  bones_wigths;

layout(location = 0) out vec4  fragcoord;
layout(location = 1) out vec3  frag_position;
layout(location = 2) out vec2  frag_texcoord;
layout(location = 3) out vec3  frag_normal;
layout(location = 4) out vec3  frag_tangent;
layout(location = 5) out vec3  frag_bitangent;
layout(location = 6) out float frag_clipdepth;

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

layout(set = 2, binding = 0) uniform ObjectBuffer
{
   	mat4 object_transform;
	uint object_bones_id_offset;
	uint material_index;
	uint unused0;
	uint unused1;
};

layout(set = 2, binding = 1) uniform BonesOffsetsBuffer
{
   	mat4 bone_offset[64];
};

layout(set = 2, binding = 2) uniform BonesPoseBuffer
{
   	mat4 bone_pose[64];
};

vec3 TransformCoVector(in vec3 co_vector_, in mat4 transform_)
{
    //Get the scaling of the transformation
    vec3 scaling = vec3(transform_[0][0] * transform_[0][0] + transform_[0][1] * transform_[0][1] + transform_[0][2] * transform_[0][2],
                        transform_[1][0] * transform_[1][0] + transform_[1][1] * transform_[1][1] + transform_[1][2] * transform_[1][2],
                        transform_[2][0] * transform_[2][0] + transform_[2][1] * transform_[2][1] + transform_[2][2] * transform_[2][2]);
    
    return vec3(vec4(co_vector_, 0.0f) * transform_) / scaling;
}

void main()
{
    mat4 transform;
    if(object_bones_id_offset != Infinite)
    {
        transform = mat4(0.0f);
        for(uint i = 0 ; i < 4 ; i ++)
            transform += bones_wigths[i] * bone_offset[bones_ids[i]] * bone_pose[bones_ids[i]];
    
        transform = transform * object_transform;
    }
    else
    {
        transform = object_transform;
    }

      vec4 absulote_position = vec4(position, 1.0f) * transform;
    

    fragcoord       = projection_matrix * (view_matrix * absulote_position);
    frag_position   = absulote_position.xyz;
    frag_texcoord   = texcoord;
    frag_normal     = TransformCoVector(normal, transform);
    frag_tangent    = vec3(vec4(tangent, 0.0f) * transform);
    frag_bitangent  = vec3(vec4(bitangent, 0.0f) * transform);
    frag_clipdepth  = dot(clip_plane.xyz, absulote_position.xyz) - clip_plane.w;

    gl_Position   = fragcoord;
}