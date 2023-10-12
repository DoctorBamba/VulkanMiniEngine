#include "PhysicsPipeline.h"


Void Physics::CPhysicsPipeline::CreateComputeQueueAndCommandsPool()
{
	Uint queueFamilyIndex = p_Device->FindQueueFamily((VkQueueFlagBits)(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT));
	vkGetDeviceQueue(p_Device->device, queueFamilyIndex, 0, &p_ComputeQueue);

	
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.pNext				= nullptr;

	poolInfo.queueFamilyIndex	= queueFamilyIndex;

	if (vkCreateCommandPool(p_Device->device, &poolInfo, nullptr, &p_ComputeCommandsPool) != VK_SUCCESS)
	{
		throw std::runtime_error("CPhysicsPipeline::CreateComputeCommandsPool -> Failed to create command pool!");
	}
}

Void Physics::CPhysicsPipeline::CreateInputLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> per_tick_bindings;

	VkDescriptorSetLayoutBinding uniform_buffer_bindings = {};
	uniform_buffer_bindings.binding					= 0;
	uniform_buffer_bindings.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_buffer_bindings.descriptorCount			= 1;
	uniform_buffer_bindings.stageFlags				= VK_SHADER_STAGE_COMPUTE_BIT;

	per_tick_bindings.push_back(uniform_buffer_bindings);

	for (int i = 1; i < 5; i++)
	{
		VkDescriptorSetLayoutBinding input_storge_buffers_bindings = {};
		input_storge_buffers_bindings.binding			= i;
		input_storge_buffers_bindings.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		input_storge_buffers_bindings.descriptorCount	= 1;
		input_storge_buffers_bindings.stageFlags		= VK_SHADER_STAGE_COMPUTE_BIT;

		per_tick_bindings.push_back(input_storge_buffers_bindings);
	}

	VkDescriptorSetLayoutCreateInfo per_frame_set_info = {};
	per_frame_set_info.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	per_frame_set_info.flags		= 0;
	per_frame_set_info.pNext		= nullptr;

	per_frame_set_info.bindingCount = per_tick_bindings.size();
	per_frame_set_info.pBindings	= per_tick_bindings.data();
	

	if (vkCreateDescriptorSetLayout(p_Device->device, &per_frame_set_info, nullptr, &descriptor_set_layouts[DescriptorSetLayouts::PerTick]) != VK_SUCCESS)
	{
		throw std::runtime_error("Physics Pipeline Error - > Failed to create descriptor set layout!");
		return;
	}


	//Pipeline Layout...
	VkPipelineLayoutCreateInfo pipeline_layout_desc = {};
	pipeline_layout_desc.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_desc.flags					= 0;
	pipeline_layout_desc.pNext					= nullptr;

	pipeline_layout_desc.setLayoutCount			= DescriptorSetLayouts::COUNT;
	pipeline_layout_desc.pSetLayouts			= descriptor_set_layouts;
	pipeline_layout_desc.pushConstantRangeCount = 0;
	pipeline_layout_desc.pPushConstantRanges	= nullptr;

	if (vkCreatePipelineLayout(p_Device->device, &pipeline_layout_desc, nullptr, &p_InputLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Physics Pipeline Input Layout faild to create!");
		return;
	}
}
