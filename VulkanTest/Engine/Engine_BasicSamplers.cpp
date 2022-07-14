#include "Engine_BasicSamplers.h"

namespace Engine::UsefulSamplers
{
	VkSampler regular_nearest;
	VkSampler regular_linear;
	VkSampler high_quality_nearest;
	VkSampler high_quality_linear;
}

Void Engine::CreateBasicSamplers(VulkanDevice* device_)
{
	VkSamplerCreateInfo sampler_desc{};
	sampler_desc.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_desc.magFilter					= VK_FILTER_NEAREST;
	sampler_desc.minFilter					= VK_FILTER_NEAREST;
	sampler_desc.addressModeU				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_desc.addressModeV				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_desc.addressModeW				= VK_SAMPLER_ADDRESS_MODE_REPEAT;

	sampler_desc.anisotropyEnable			= VK_FALSE;
	sampler_desc.maxAnisotropy				= 0;
	sampler_desc.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_desc.unnormalizedCoordinates	= VK_FALSE;
	sampler_desc.compareEnable				= VK_FALSE;
	sampler_desc.compareOp					= VK_COMPARE_OP_NEVER;

	sampler_desc.mipmapMode					= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_desc.mipLodBias					= 0.0f;
	sampler_desc.minLod						= 0.0f;
	sampler_desc.maxLod						= 0.0f;

	if (vkCreateSampler(device_->device, &sampler_desc, nullptr, &UsefulSamplers::regular_nearest) != VK_SUCCESS)
	{
		throw std::runtime_error("Engine :: CreateBasicSamplers -> Failed to create the 'regular_nearest' sampler!");
		return;
	}

	sampler_desc.magFilter = VK_FILTER_LINEAR;
	sampler_desc.minFilter = VK_FILTER_LINEAR;

	if (vkCreateSampler(device_->device, &sampler_desc, nullptr, &UsefulSamplers::regular_linear) != VK_SUCCESS)
	{
		throw std::runtime_error("Engine :: CreateBasicSamplers -> Failed to create the 'regular_linear' sampler!");
		return;
	}

	sampler_desc.anisotropyEnable			= VK_TRUE;
	sampler_desc.maxAnisotropy				= device_->properties.limits.maxSamplerAnisotropy; //For high quality

	if (vkCreateSampler(device_->device, &sampler_desc, nullptr, &UsefulSamplers::high_quality_linear) != VK_SUCCESS)
	{
		throw std::runtime_error("Engine :: CreateBasicSamplers -> Failed to create the 'high_quality_linear' sampler!");
		return;
	}

	sampler_desc.magFilter = VK_FILTER_NEAREST;
	sampler_desc.minFilter = VK_FILTER_NEAREST;

	if (vkCreateSampler(device_->device, &sampler_desc, nullptr, &UsefulSamplers::high_quality_nearest) != VK_SUCCESS)
	{
		throw std::runtime_error("Engine :: CreateBasicSamplers -> Failed to create the 'high_quality_linear' sampler!");
		return;
	}
}