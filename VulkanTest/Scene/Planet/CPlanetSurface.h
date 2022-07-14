#pragma once
#include "../../Engine/Engine_Renderer.h"

class CPlanetSurface : public CObject
{
	struct Material
	{
		Uint high_map_texloc;
		Uint rock_color_texloc;
		Uint rock_normal_texloc;
		Uint ground_color_texloc;
		Uint padding[252];
	};

	private:
		Float radious;
		CTextureCube* high_map;

	public:
		CPlanetSurface();

		Void GeneratePlanetMap(CGpuUploadTask* upload_task_);

		Void Init(CGpuUploadTask* upload_task_) override;
		Void Render(CGpuDrawTask* draw_task_, Uint layer_) override;
};