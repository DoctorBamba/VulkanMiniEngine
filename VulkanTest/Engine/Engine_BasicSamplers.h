#pragma once
#include "../Graphics/CDevice.h"

namespace Engine
{
	namespace UsefulSamplers
	{
		extern VkSampler regular_nearest;
		extern VkSampler regular_linear;
		extern VkSampler high_quality_nearest;
		extern VkSampler high_quality_linear;
	};

	Void CreateBasicSamplers(CVulkanDevice* device_);
}