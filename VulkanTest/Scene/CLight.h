#pragma once

#include "CCamera.h"
#include "../Graphics/CFrameBuffer.h"

#define DEPTHMAP_UNKNOW  0
#define DEPTHMAP_REGULAR 1
#define DEPTHMAP_CUBE    2


enum ShadowTechnique
{
	SHADOW_TECHNIQUE_UNKNOW,
	SHADOW_TECHNIQUE_DEPTHMAP,
	SHADOW_TECHNIQUE_VOLUME,
	SHADOW_TECHNIQUE_RAYTRACING
};

struct DepthmapProperties
{
	Uint  process_depthmap;
	Uint  shadow_samples_count;
	Float shadow_depth_bias;
	Float shadow_blurring_factor;

	Uint  flag_samples_count;
	Float flag_samples_advance;
	Float near_plane;
	Float far_plane;

	DepthmapProperties()
	{
		process_depthmap		= DEPTHMAP_UNKNOW;
		shadow_samples_count	= 4;
		shadow_depth_bias		= 1.0f;
		shadow_blurring_factor	= 1000.0f;

		flag_samples_count		= 0;
		flag_samples_advance = 0.0f;
		near_plane				= 1.0f;
		far_plane				= 100.0f;
	}
};

class CLight : public virtual CObject
{
	protected:
		Vector4D		color;
		Float		distance;
		Float		fallout_const;
		Float		fallout_linear;
		Float		fallout_sqr;
		Float		radius;

		ShadowTechnique shadow_technique;

		Uint light_id;

	public:
		
		CLight()  : CObject("New_Light"), light_id(Engine::objects_manager->NewLightIdentefier())
		{
			color				= Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
			distance			= 10.0f;
			fallout_const		= 0.0f;
			fallout_linear		= 0.0f;
			fallout_sqr			= 1.0f;
			radius				= 0.5f;

			shadow_technique	= SHADOW_TECHNIQUE_UNKNOW;
		}

		Vector3D	GetAbsolutePosition()
		{
			Matrix4D transform = GetTransform();
			return Vector3D(transform[0][3], transform[1][3], transform[2][3]);
		}

		Vector4D GetColor() { return this->color; }
		Float	GetDistance() { return this->distance; }


		Void SetColor(const Vector4D& color_) { this->color = color_; }
		Void	SetDistance(const DOUBLE distance_) { this->distance = distance_; }
		Void SetFallout(Float fallout_const_, Float fallout_linear_, Float fallout_sqr_)
		{
			this->fallout_const		= fallout_const_;
			this->fallout_linear	= fallout_linear_;
			this->fallout_sqr		= fallout_sqr_;
		}

		virtual Engine::GpuLightStruct GetGpuStruct() { return Engine::GpuLightStruct(); }

		ShadowTechnique GetShadowTechnique() { return shadow_technique; }

		Void SetRadius(const Float radius_) { this->radius = radius_; }
		Float GetRadius() { return this->radius; }

		Uint GetLightId() { return light_id; }

};

class CSpotLight : public CLight, public CPersCamera
{
	private:
		CFrameBuffer*      depthmap_framebuffer;
		DepthmapProperties depthmap_properties;

	public:

		CSpotLight() : CPersCamera(), CLight()
		{
			name = "New_Spot_Light";

			depthmap_framebuffer = nullptr;
		
			SetZlimits(1.0f, 100.0f);
			SetLookPoint(Vector3D(-1.0f, 0.0f, 0.0f));
			SetUp(Vector3D(0.0f, 0.0f, 1.0f));
			SetSolidAngles(1.2f, 1.2f);
		}

		CSpotLight(CObject* source_) : CObject(source_), CPersCamera(), CLight()
		{
			name = "New_Spot_Light";

			depthmap_framebuffer = nullptr;
		
			SetZlimits(1.0f, 100.0f);
			SetLookPoint(Vector3D(-1.0f, 0.0f, 0.0f));
			SetUp(Vector3D(0.0f, 0.0f, 1.0f));
			SetSolidAngles(1.2f, 1.2f);
		}

		Void ProcessDepthMap(Uint16 width_, Uint16 height_, DepthmapProperties depthmap_properties_)
		{
			/*
			if (depthmap_framebuffer != nullptr)
				delete depthmap_framebuffer;

			depthmap_framebuffer = new CFrameBuffer(device_->device, new CTexture2D(, TF_UNKNOWN, format_, device_);

			for (PE_Uint i = 0; i < Engine::graphics->GetBackBuffersNumber() ; i++)
				descriptors_blocks[i]->Bind(depthmap_target->Get_DepthStencilSurface(), 1);


			depthmap_properties = depthmap_properties_;
			shadow_technique	= SHADOW_TECHNIQUE_DEPTHMAP;
			*/
		}

		Engine::GpuLightStruct GetGpuStruct()
		{
			Vector3D position = GetAbsolutePosition();

			Engine::GpuLightStruct buffer;

			buffer.position				= Make4D(position);

			buffer.color				= color;

			buffer.distance				= distance;
			buffer.fallout_const		= fallout_const;
			buffer.fallout_linear		= fallout_linear;
			buffer.fallout_sqr			= fallout_sqr;

			buffer.direction			= direction;
			buffer.cone_angle			= tan(0.5f * cone_angle);
		
			buffer.depthmap_preporties.process_depthmap			= (shadow_technique == SHADOW_TECHNIQUE_DEPTHMAP) ? DEPTHMAP_REGULAR : DEPTHMAP_UNKNOW;
			buffer.depthmap_preporties.shadow_samples_count		= depthmap_properties.shadow_samples_count;
			buffer.depthmap_preporties.shadow_bias				= depthmap_properties.shadow_depth_bias;
			buffer.depthmap_preporties.shadow_bluring_factor	= depthmap_properties.shadow_blurring_factor;
			buffer.depthmap_preporties.flag_samples_count		= depthmap_properties.flag_samples_count;
			buffer.depthmap_preporties.flag_samples_advance		= depthmap_properties.flag_samples_advance;
			buffer.depthmap_preporties.near_plane				= depthmap_properties.near_plane;
			buffer.depthmap_preporties.far_plane				= depthmap_properties.far_plane;

			Matrix4D view = LookAt<Float>(position, position + direction, up);
			Matrix4D proj = Perspective<Float>(cone_angle, 1.0f, depthmap_properties.near_plane, depthmap_properties.far_plane);
			Transpose<Float>(proj * view, &buffer.mvp);

			return buffer;
		}

		//CRenderTarget* GetTarget() { return depthmap_target; }
};

/*
class CSourceLight : public CLight
{
	private:

		CRenderTarget*     depthmap_targets[6];
		DepthmapProperties depthmap_properties;


	public:

	CSourceLight() : CLight()
	{
		name			= "New_Sorce_Light";
		ZeroMemory(depthmap_targets, sizeof(CRenderTarget*) * 6);
	}

	CSourceLight(CObject* source_) : CObject(source_), CLight()
	{
		name = "New_Sorce_Light";
		ZeroMemory(depthmap_targets, sizeof(CRenderTarget*) * 6);
	}

	PE_Void EnableShadowMap(PE_Uint16 width_, PE_Uint16 height_, TextureFormats format_, DepthmapProperties depthmap_properties_, ID3D12Device* device_)
	{
		for(PE_Uint i = 0 ; i < 6 ; i ++)
		{
			if (depthmap_targets[i] != nullptr)
				delete depthmap_targets[i];

			depthmap_targets[i] = new CRenderTarget(width_, height_, TF_UNKNOWN, format_, device_);
			
			for (PE_Uint j = 0; j < Engine::graphics->GetBackBuffersNumber(); j++)
				descriptors_blocks[j]->Bind(depthmap_targets[i]->Get_DepthStencilSurface(), 1 + i);//From 1 to 6 in the per light descriptor
		}

		depthmap_properties = depthmap_properties_;
		shadow_technique	= SHADOW_TECHNIQUE_DEPTHMAP;
	}

	Engine::GpuLightStruct GetGpuStruct()
	{
		PE_Vector3D position = GetAbsolutePosition();

		Engine::GpuLightStruct buffer;

		buffer.position				= PE_Make4D(position);

		buffer.color				= color;

		buffer.distance				= distance;
		buffer.fallout_const		= fallout_const;
		buffer.fallout_linear		= fallout_linear;
		buffer.fallout_sqr			= fallout_sqr;

		buffer.direction			= PE_Vector3D(0.0f, 0.0f, 0.0f);
		buffer.cone_angle			= 0.0f;
		
		buffer.depthmap_preporties.process_depthmap			= (shadow_technique == SHADOW_TECHNIQUE_DEPTHMAP) ? DEPTHMAP_CUBE : DEPTHMAP_UNKNOW;
		buffer.depthmap_preporties.shadow_samples_count		= depthmap_properties.shadow_samples_count;
		buffer.depthmap_preporties.shadow_bias				= depthmap_properties.shadow_depth_bias;
		buffer.depthmap_preporties.shadow_bluring_factor	= depthmap_properties.shadow_blurring_factor;
		buffer.depthmap_preporties.flag_samples_count		= depthmap_properties.flag_samples_count;
		buffer.depthmap_preporties.flag_samples_advance		= depthmap_properties.flag_samples_advance;
		buffer.depthmap_preporties.near_plane				= depthmap_properties.near_plane;
		buffer.depthmap_preporties.far_plane				= depthmap_properties.far_plane;

		buffer.mvp[0][0] = depthmap_properties.near_plane;//Source Lights use mvp matrix only for store near and far planes
		buffer.mvp[1][1] = depthmap_properties.far_plane;//Source Lights use mvp matrix only for store near and far planes

		return buffer;
	}

	CRenderTarget* GetTarget(PE_Uint index_) { return depthmap_targets[index_]; }

};
*/