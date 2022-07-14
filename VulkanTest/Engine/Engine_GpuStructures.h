#pragma once

#include "../Workspace.h"

class CPbrMaterial;

namespace Engine
{
	//*************** GPU Structures ********************///
	struct GpuLookStruct
	{
		Vector4D camera_position;
		Vector4D camera_direction;
		Matrix4D view_matrix;
		Matrix4D projection_matrix;
		Vector4D clip_plane;
		Float	 time;
		Float	 unused0;
		Float	 unused1;
		Float	 unused2;
	};

	struct GpuDataBlockStruct
	{
		Byte bytes[256];
	};

	struct GpuPbrMaterialStruct
	{
		Vector4D base_color;

		Float	 metalic;
		Float	 roughness;
		Float	 specular;
		Float	 transmision;

		Vector3D emmision;
		Float	 emmision_power;

		Uint base_color_texture_loc;
		Uint metalic_texture_loc;
		Uint roughness_texture_loc;
		Uint specular_texture_loc;
		Uint transmision_texture_loc;
		Uint transmision_roughness_texture_loc;
		Uint emission_texture_loc;
		Uint normals_texture_loc;
	};

	struct GpuObjectStruct
	{
		Matrix4D mesh_transform;
		Uint	 bones_ids_offset;
		Uint	 material_index;
		Uint		a;
		Uint		b;
	};

	struct GpuArmatureStruct
	{
		Float actived_deriation;
		Float actived_time;
		Uint  bones_ids_offset;
		Uint  A;
	};

	struct GpuLightStruct
	{
		Vector4D position;
		Vector4D color;

		Float	distance;
		Float	fallout_const;
		Float	fallout_linear;
		Float	fallout_sqr;

		Vector3D direction;
		Float	 cone_angle;

		struct DepthmapPreporties
		{
			Uint	process_depthmap;
			Uint	shadow_samples_count;
			Float	shadow_bias;
			Float	shadow_bluring_factor;
			Uint	flag_samples_count;
			Float	flag_samples_advance;
			Float	near_plane;
			Float	far_plane;
		}depthmap_preporties;

		Matrix4D mvp;
	};

	struct GpuEmitterStruct
	{
		Vector3D position;
		Float	radius;
		Vector4D color;
	};

	struct GpuPointStruct
	{
		Vector3D position;
		Vector4D color;
		Float	radius;
	};


	struct GpuPropertiesBlockStruct
	{
		Byte bytes[256];
	};

	//Builders...

	GpuPbrMaterialStruct BuildGpuMaterialStruct(CPbrMaterial* material_);
}