#pragma once
#include "../CGraphics.h"
#include "Engine_GpuStructures.h"
#include "Engine_BasicSamplers.h"
#include "ResourcesManagers/Engine_ResourcesManager.h"

namespace Engine
{
	extern CGraphics* graphics;

	Void SetViewport(VkCommandBuffer command_buffer_, Float x_, Float y_, Float width_, Float height_);
}