#pragma once

#include "CGameObject.h"

class CCamera : public virtual CObject
{
	protected:

		Vector3D direction;
		Vector3D up;
		Float	 near_plane;
		Float	 far_plane;

		Matrix4D view_matrix;
		Matrix4D projection_matrix;

		Vector4D clip_plane;
		Float	 time;


	//Flush functions...

	virtual void UpdateViewMatrix() { view_matrix = LookAt<FLOAT>(CObject::GetPosition(), direction, up); }
	virtual void UpdateProjectionMatrix() { projection_matrix = Matrix4D(1.0f); };

	public:

		CCamera() : CObject("New_Camera")
		{
			direction	= Vector3D(-1.0f, -1.0f, -1.0f);
			up			= Vector3D(0.0f, 0.0f, 1.0f);
			near_plane	= 1.0f;
			far_plane	= 100.0f;

			clip_plane	= Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
			time		= 0.0f;

			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

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
};

class CPersCamera : public CCamera
{
	protected:
		Float cone_angle;
		Float aspect;

	public:

		CPersCamera() : CCamera()
		{
			cone_angle = 0.0f;
			aspect	   = 1.0f;
		}
		
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