#pragma once

#include "CAllocator.h"

#define MAXIMUM_DESCRIPTOR_SETS_PER_POOL 1000000

class CDescriptorPool
{
	protected:
        VkDescriptorPool p_DescriptorPool;

        const VkDevice p_Device;

	public:
        CDescriptorPool(VkDevice device_, Uint descriptors_count_) : p_Device(device_)
		{
            VkDescriptorPoolSize uniforms_size{};
            uniforms_size.type              = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniforms_size.descriptorCount   = descriptors_count_;

            VkDescriptorPoolSize images_size{};
            images_size.type            = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            images_size.descriptorCount = descriptors_count_;

            VkDescriptorPoolSize pool_sizes[] = { uniforms_size, images_size };

            VkDescriptorPoolCreateInfo pool_desc{};
            pool_desc.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_desc.poolSizeCount = array_size(pool_sizes);
            pool_desc.pPoolSizes    = pool_sizes;
            pool_desc.maxSets       = MAXIMUM_DESCRIPTOR_SETS_PER_POOL;

            if (vkCreateDescriptorPool(p_Device, &pool_desc, nullptr, &p_DescriptorPool) != VK_SUCCESS)
            {
                throw std::runtime_error("CDescriptorSet::Constructor -> Failed to create the descriptor pool!");
                return;
            }
		}
};