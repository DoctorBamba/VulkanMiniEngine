#pragma once
#include "Engine_Renderer.h"
#include "Engine_BasicMeshs.h"
#include "Engine_BasicSamplers.h"
#include "ResourcesManagers/Engine_ResourcesManager.h"

namespace Engine
{
	Void Install();
	Void Clean();

	Void Init_Event(CGpuUploadTask* upload_task_);
	Void Draw_Event(CGpuDrawTask* draw_task_, CFrameBuffer* target_);
}