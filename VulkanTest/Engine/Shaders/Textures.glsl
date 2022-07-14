#define Infinite 0xffffffffu

layout(set = 0, binding = 1) uniform sampler2D   textures[];
layout(set = 0, binding = 1) uniform samplerCube cubemaps[];

vec4 SampleColor(sampler2D texture_, vec2 uv_)
{
    return texture(texture_, uv_);
}

float SampleScalar(sampler2D texture_, in vec2 uv_)
{
    return texture(texture_, uv_).r;
}

vec3 SampleXYZ(sampler2D texture_, in vec2 uv_)
{
    uv_.y = 1.0 - uv_.y;
    return (2.0 * texture(texture_, uv_).xyz) - vec3(1.0, 1.0, 1.0);
}

vec3 ComputeNormalFromMap(in uint texture_loc_, in vec2 uv_, in vec3 tangent_, in vec3 bitangent_, in vec3 normal_, in float normal_factor)
{
    if (texture_loc_ != Infinite)
    {
        vec3 tangent_space_coords = SampleXYZ(textures[texture_loc_], uv_);
        return normalize(tangent_space_coords[0] * tangent_ + tangent_space_coords[1] * bitangent_ + tangent_space_coords[2] * normal_factor * normal_);
    }
    
    return normalize(normal_);
}