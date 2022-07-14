#version 450
//Layout specification.
layout (vertices = 4) out;//4 Control point per pacth
 

/*****Shader inputs*****/

//Attributes...
layout (location = 0) in vec3 cp_position[];
layout (location = 1) in vec2 cp_texcoord[];

//Buffers...
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

//Push Constant...
layout(push_constant) uniform PushConstant
{
    uint high_map_location;
};


/*****Shader output*****/
layout (location = 0) out vec3 out_position[];
layout (location = 1) out vec2 out_texcoord[];



/*****Functions*****/
float LengthSquared(in vec3 vec)
{
    return dot(vec, vec);
}

float GetTesselationLevel(float dis_sqr)
{
    if(dis_sqr < 16 * 16)
        return 48;
    if(dis_sqr < 36 * 36)
        return 24;
    if(dis_sqr < 128 * 128)
        return 8;
    else
        return 1;
}
 
 /*****Entry*****/
void main()
{
    //Pass along the values to the tessellation evaluation shader...
    out_position[gl_InvocationID]   = cp_position[gl_InvocationID];
    out_texcoord[gl_InvocationID]   = cp_texcoord[gl_InvocationID];
 
    //Calculate the tessellation levels...
    if (gl_InvocationID == 0)
    {
        //The object's transform relevant parameters...
        float planet_radius  = object_transform[0][0];
        vec3 planet_position = object_transform[3].xyz;

        vec3 center = normalize(0.25f * (cp_position[0] + cp_position[1] + cp_position[2] + cp_position[3]));

        float tesselation_level = GetTesselationLevel(LengthSquared(camera_position.xyz - (planet_position + center * planet_radius)));

        
        gl_TessLevelInner[0] = tesselation_level;
        gl_TessLevelInner[1] = tesselation_level;
        gl_TessLevelOuter[0] = tesselation_level;
        gl_TessLevelOuter[1] = tesselation_level;
        gl_TessLevelOuter[2] = tesselation_level;
        gl_TessLevelOuter[3] = tesselation_level;
    }
}