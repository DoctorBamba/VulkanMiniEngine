#pragma once
#include "../../Engine/Engine_Renderer.h"

class CPlanetAtmosphere : public CObject
{
	private:
		CPipeline* pipeline;
		Float radious;

		struct ShaderInput
		{
			Vector3D	planet_center;
			Float		planet_radius;

			Vector3D	atmosphere_rayleigh_coeff;
			Float		atmosphere_radius;
			Vector3D	atmosphere_mie_coeff;
			Float		atmosphere_mie_scatt_coeff;

			Vector3D	sun_direction;
			Float		sun_intensity;
			Vector4D	sun_color;
		};

	public:
		CPlanetAtmosphere(Float radious_);

		Void Init(CGpuUploadTask* upload_task_) override;
		Void Render(CGpuDrawTask* draw_task_, Uint layer_) override;
};