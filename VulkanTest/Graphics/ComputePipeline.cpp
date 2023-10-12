#include "ComputePipeline.h"

VkPipelineLayout CComputePipeline::DefaultPipelineLayout;

/*
Void CComputePipeline::CreateDefaultComputePipelineLayout()
{
	//Per frame descriptor set...

	VkDescriptorSetLayoutBinding per_frame_binds[24];

	VkDescriptorSetLayoutBinding& storge_buffers_bindings = per_frame_binds[1];
	storge_buffers_bindings					= {};
	storge_buffers_bindings.binding			= 1;
	storge_buffers_bindings.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	storge_buffers_bindings.descriptorCount = 1;
	storge_buffers_bindings.stageFlags		= VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding& uniform_buffers_bindings = per_frame_binds[0];
	uniform_buffers_bindings = {};
	uniform_buffers_bindings.binding				= 0;
	uniform_buffers_bindings.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_buffers_bindings.descriptorCount		= 1;
	uniform_buffers_bindings.stageFlags				= VK_SHADER_STAGE_ALL;

}
*/


CComputePipeline::CComputePipeline(CVulkanDevice* device_, std::string name_, VkPipelineShaderStageCreateInfo shader_info_, VkPipelineLayout customLayout) : p_Device(device_)
{
	//if (CComputePipeline::DefaultPipelineLayout == nullptr && customLayout == nullptr)
		//CreateDefaultComputePipelineLayout();


	VkComputePipelineCreateInfo compute_pipeline_info = {};
	compute_pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	compute_pipeline_info.flags = 0;
	compute_pipeline_info.pNext = nullptr;

	compute_pipeline_info.layout = (customLayout != nullptr) ? customLayout : CComputePipeline::DefaultPipelineLayout;
	compute_pipeline_info.stage = shader_info_;
	compute_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	if (!vkCreateComputePipelines(p_Device->device, VK_NULL_HANDLE, 1, &compute_pipeline_info, VK_NULL_HANDLE, &this->p_Pipeline))
	{
		throw std::runtime_error("Compute Pipeline '" + name_ + "' failed to create!'");
		return;
	}
}

Void CComputePipeline::Dispacth(CGpuTask* compute_task_, Uint group_count_x_, Uint group_count_y_, Uint group_count_z_)
{
	vkCmdBindPipeline(compute_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, p_Pipeline);
	vkCmdDispatch(compute_task_->GetCommandBuffer(), group_count_x_, group_count_y_, group_count_z_);
}