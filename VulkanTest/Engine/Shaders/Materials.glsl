#include "Lights.glsl"
#include "Textures.glsl"

struct Material
{
    vec4 base_color;

    float metalic;
    float roughness;
    float specular;
    float transmision;

    vec3 emmision;
    float emmision_power;

    uint base_color_texture_loc;
    uint metalic_texture_loc;
    uint roughness_texture_loc;
    uint specular_texture_loc;
    uint transmision_texture_loc;
    uint transmision_roughness_texture_loc;
    uint A;
    uint normals_texture_loc;

    uint p[44];
};


//Basic functions of material

vec4 SurfaceIntensityFromLight(in Light light_, in vec4 surface_color_, in vec4 surface_mrst_, in vec3 surface_position_, in vec3 surface_normal_, in vec3 camera_position_, in vec3 camera_direction_)
{
    vec3 surface_to_light   = normalize(light_.position.xyz - surface_position_);
    vec3 surface_to_camera  = normalize(camera_position_ - surface_position_);
    vec3 wiew_direction     = normalize(camera_direction_ - camera_position_);
    
    float diffuse_coefficient = max(0.0, dot(surface_normal_, surface_to_light));
    
	float specular_coefficient = 0.0;
	if (diffuse_coefficient > 0.0)
	{
        vec3 reflect_direction = normalize(-surface_to_light + 2 * surface_normal_ * dot(surface_normal_, surface_to_light));
        specular_coefficient = surface_mrst_.z * pow(clamp(-dot(wiew_direction, reflect_direction), 0.0, 1.0), 100.0 * (1.0 - surface_mrst_.y) * (1.0 - surface_mrst_.y));
    }

	//Emssi Color
	
	//Atten Color
    vec3  distance_ = light_.position.xyz - surface_position_;
    float normalize_distance = dot(distance_, distance_) / (light_.range * light_.range);
    float attenuation = light_.fallout_const + light_.fallout_linear / sqrt(normalize_distance) + light_.fallout_sqr / normalize_distance;
	
    return light_.color * vec4(attenuation * (diffuse_coefficient * surface_color_.rgb), 1.0);
}