#pragma once
#include "../Workspace.h"
#include "CMemorySpace.h"

#define NOT_AVAILABLE 0xffffffff

class CVulkanDevice
{
	public:
		static const Uint MaximumDescriptorSetsCount = 1000000;

	private:
		VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

	public:
		VkDevice					device;
		VkPhysicalDevice			physical_device;
		VkPhysicalDeviceProperties	properties;
		

		struct DeviceMemory
		{
			CMemorySpace* uniform_space;
			CMemorySpace* local_space;
		}MemorySpaces;

		Void AllocateMemorySpaces()
		{
			MemorySpaces.uniform_space	= new CMemorySpace(this, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, __128MB);
			MemorySpaces.local_space	= new CMemorySpace(this, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, __128MB);
			MemorySpaces.uniform_space->Map();//Map uniform memory becuse we need to update this memory repadly
		}

		Uint FindQueueFamily(VkQueueFlagBits queue_flags_) const
		{
			//Serche for Physical Queues Families...
			uint32_t queue_family_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);//Only get the number

			std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

			Uint family_index = NOT_AVAILABLE;

			for (Uint i = 0; i < queue_family_count; i++)
			{
				if ((queue_families.at(i).queueFlags & queue_flags_) == queue_flags_)
				{
					family_index = i;
					break;
				}
			}

			if (family_index == NOT_AVAILABLE)
				throw std::runtime_error("Device Error -> Not found a family that support the given Queue-Type");

			return family_index;
		}

		VkQueue GetQueue(VkQueueFlagBits queue_flags_)//Return the first queue which support the requird queue flags
		{
			VkQueue queue;
			vkGetDeviceQueue(device, FindQueueFamily(queue_flags_), 0, &queue);
			return queue;
		}

		Uint FindSupportedMemoryType(Uint memory_type_flag_, Uint addition_type_requirments_) const
		{
			VkPhysicalDeviceMemoryProperties memory_prop;
			vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_prop);

			for (Uint i = 0; i < memory_prop.memoryTypeCount; i++)
			{
				Uint bit_mask = (1u << i);
				if ((memory_type_flag_ & bit_mask) && ((memory_prop.memoryTypes[i].propertyFlags & addition_type_requirments_) == addition_type_requirments_))
					return i;
			}

			throw std::runtime_error("Device Error -> Faild to find a devices' memory with memory type that support the requirment of this buffer!");
			return 0;
		}

		VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout descriptor_set_layout_)
		{
			VkDescriptorSet descriptor_set;

			if (descriptor_pool == VK_NULL_HANDLE)//If there is no avalible descriptor pool then create a new one
			{
				const VkDescriptorPoolSize DescriptorPoolSizes[] = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 65536 },
																	 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024},
																	 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1024},
																	 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024},
																	 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 128} };

				VkDescriptorPoolCreateInfo pool_desc{};
				pool_desc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				pool_desc.flags = 0;
				pool_desc.pNext = nullptr;

				pool_desc.poolSizeCount = array_size(DescriptorPoolSizes);
				pool_desc.pPoolSizes = DescriptorPoolSizes;
				pool_desc.maxSets = MaximumDescriptorSetsCount;

				if (vkCreateDescriptorPool(device, &pool_desc, nullptr, &descriptor_pool) != VK_SUCCESS)
				{
					throw std::runtime_error("Device Error -> Failed to create the main Descriptor Pool!");
					return {};
				}
			}

			//Alocate descriptor sets...
			VkDescriptorSetAllocateInfo allocation_info = {};
			allocation_info.sType						= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocation_info.pNext						= nullptr;

			allocation_info.pSetLayouts					= &descriptor_set_layout_;
			allocation_info.descriptorPool				= descriptor_pool;
			allocation_info.descriptorSetCount			= 1;

			if (vkAllocateDescriptorSets(device, &allocation_info, &descriptor_set) != VK_SUCCESS)
			{
				throw std::runtime_error("Device Error -> Failed to allocate descriptor sets!");
				return {};
			}

			return descriptor_set;
		}
};