#pragma once
#include "../Workspace.h"
#include "CGpuTask.h"
#include "CDevice.h"


class CComputePipeline
{
	private:
		std::string m_Name;

		VkPipeline p_Pipeline;

		static VkPipelineLayout DefaultPipelineLayout;//In my Engine I decided to fix the input layout of a compute pipeline.
	
		CVulkanDevice* const p_Device;
	public:
		CComputePipeline(CVulkanDevice* device_, std::string name_, VkPipelineShaderStageCreateInfo shader_info_, VkPipelineLayout customLayout = nullptr);
		static Void CreateDefaultComputePipelineLayout();
		Void QQ(){}
		Void Dispacth(CGpuTask* compute_task_, Uint thread_groups_x_, Uint thread_groups_y_, Uint thread_groups_z_);
};