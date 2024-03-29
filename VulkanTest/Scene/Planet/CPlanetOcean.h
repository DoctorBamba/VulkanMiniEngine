#pragma once
#include "../../Engine/Engine_Renderer.h"

class CPlanetOcean: public CObject
{
	private:
		CPipeline* pipeline;
		
		CPersCamera* reflection_layer_camera;
		Uint reflection_layer_id;

		Float radious;

		struct DataBlock
		{
			Uint reflection_texture_loc;
			Uint A;
			Uint B;
			Uint C;
		};

		std::vector<CPbrMaterial*> materials_packet;

		CPersCamera* reflection_camera;
	public:
		CPlanetOcean();

		Void Init(CGpuUploadTask* upload_task_) override;
		Void Draw(CGpuDrawTask* draw_task_, CCamera* camera) override;
};