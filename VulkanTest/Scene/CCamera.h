#pragma once

#include "CGameObject.h"

#include "../Graphics/CFrameBuffer.h"

class CCamera : public virtual CObject
{
	public:
		enum class RenderMode
		{
			Forward,
			Deffered
		};

		struct PostProcessingInvocation
		{
			CPipeline*							pipeline;
			Engine::GpuPropertiesBlockStruct	properties_block;
		};

	protected:

		Vector3D direction;
		Vector3D up;
		Float	 near_plane;
		Float	 far_plane;

		Matrix4D view_matrix;
		Matrix4D projection_matrix;

		Vector4D clip_plane;
		Float	 time;

		RenderMode renderMode;
		CFrameBuffer* frame_buffer;
		std::list<PostProcessingInvocation> pp_invocations;


		Uint layer;
		Uint camera_id;

	//Flush functions...

	virtual void UpdateViewMatrix() { view_matrix = LookAt<Float>(CObject::GetPosition(), direction, up); }
	virtual void UpdateProjectionMatrix() { projection_matrix = Matrix4D(1.0f); };

	public:

		//Create a new camera used existed Frame Buffer.
		CCamera(CFrameBuffer* frame_buffer_) : CObject("New_Camera"), camera_id(Engine::objects_manager->NewCameraIdentefier())
		{
			direction	= Vector3D(-1.0f, -1.0f, -1.0f);
			up			= Vector3D(0.0f, 0.0f, 1.0f);
			near_plane	= 1.0f;
			far_plane	= 100.0f;

			clip_plane	= Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
			time		= 0.0f;

			frame_buffer = frame_buffer_;

			layer		= 0;

			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		//Create a new camera with defualt Frame Buffer.
		CCamera(CVulkanDevice* device, Uint16 resolution_width, Uint resolution_height) : CCamera(
				new CFrameBuffer(device->device, 
								{ (CTextureBase*)(new CTexture2D(device->MemorySpaces.local_space, resolution_width, resolution_height, VK_FORMAT_R8G8B8A8_UNORM, true)) },
								  (CTextureBase*)(new CTexture2D(device->MemorySpaces.local_space, resolution_width, resolution_height, VK_FORMAT_D32_SFLOAT_S8_UINT, true)),
								  new CRenderPass(device->device, { VK_FORMAT_R8G8B8A8_UNORM }, VK_FORMAT_D32_SFLOAT_S8_UINT, RenderPassType::Texture2D))){}

		CCamera() : CCamera(nullptr) {}

		//Set functions...

		Void SetLookPoint(const Vector3D& direction_) { this->direction = direction_; }
		Void SetUp(const Vector3D& up_) { this->up = up_; }
		Void SetZlimits(Float near_, Float far_) { this->near_plane = near_; this->far_plane = far_; }
		Void SetTime(Float time_) { this->time = time_; }
		Void SetClipPlane(const Vector4D& plane_) { this->clip_plane = plane_; }

		//Get functions...

		Engine::GpuLookStruct GetLookBuffer()
		{
			//UpdatetWorldMatrix();
			UpdateViewMatrix();
			UpdateProjectionMatrix();

			Vector3D position = CObject::GetPosition();

			Engine::GpuLookStruct buffer;
			
			buffer.camera_position		= Vector4D(position[0], position[1], position[2], 1.0f);
			buffer.camera_direction		= Vector4D(direction[0], direction[1], direction[2], 1.0f);
			Transpose<Float>(view_matrix, &buffer.view_matrix);
			Transpose<Float>(projection_matrix, &buffer.projection_matrix);
			buffer.clip_plane	= clip_plane;
			buffer.time			= time;

			return buffer;
		}

		Vector3D GetUp() { return this->up; }
		Vector3D GetLookPoint() { return this->direction; }
		Void GetZLimits(Float* near_, Float* far_) { *near_ = this->near_plane; *far_ = this->far_plane; }

		Uint GetLayer() { return this->layer; }
		Uint GetCameraId() { return this->camera_id; }

		CFrameBuffer* GetFrameBuffer() { return this->frame_buffer; }
		Void SetFrameBuffer(CFrameBuffer* frame_buffer_) { this->frame_buffer = frame_buffer_; }


		//Postprocessing invocations...
		Void AddPostProcessingInvocation(CPipeline* pipeline_, Engine::GpuPropertiesBlockStruct properties_block_) { pp_invocations.push_back({ pipeline_ , properties_block_ }); }
		Void ResetPostProcessingInvocation() { pp_invocations.clear(); }

		const std::list<PostProcessingInvocation> GetPostPocessingInvocations() { return this->pp_invocations; }
};

class CPersCamera : public CCamera
{
	protected:
		Float cone_angle = 0.0f;
		Float aspect	 = 1.0f;

	public:
		
		using CCamera::CCamera;

		//Fluse functions...

		Void UpdateProjectionMatrix()
		{
			projection_matrix = Perspective<FLOAT>(cone_angle, aspect, near_plane, far_plane);
		}

		//Set functions...

		Void SetSolidAngles(Float hcone_angle_, Float vcone_angle_)
		{
			this->cone_angle = hcone_angle_;
			this->aspect = tan(vcone_angle_) / tan(hcone_angle_);
		}
};