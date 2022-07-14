struct DepthmapProperties
{
    uint  process_depthmap;
    uint  depth_samples_count;
    float depth_bias;
    float bluring;
};

struct Light
{
    vec4 position;
    vec4 color;
    
    float range;
    float fallout_const;
    float fallout_linear;
    float fallout_sqr;
    
    vec3   _direction;
    float  tan_cone_angle;
    
    DepthmapProperties depthmap_preporties;
    
    mat4 mvp;
};