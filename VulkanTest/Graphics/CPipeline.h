#pragma once

#include "CRenderPass.h"


struct VertexInputAttribute
{
	Uint				binding;
	Uint				location;
	VkFormat			format;
	VkVertexInputRate	input_rate;
};

struct TesselationStage
{
	VkPipelineTessellationStateCreateInfo	TesselationState;
	VkPipelineShaderStageCreateInfo			TesselationControlsStage;
	VkPipelineShaderStageCreateInfo			TesselationEvaluationStage;

	TesselationStage()
	{
		TesselationState			= {};
		TesselationControlsStage	= {};
		TesselationEvaluationStage	= {};
	}

	TesselationStage(Uint cp_count_, VkPipelineShaderStageCreateInfo tess_control_stage_, VkPipelineShaderStageCreateInfo tess_evaluation_stage_)
	{
		TesselationState.sType				= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		TesselationState.pNext				= nullptr;
		TesselationState.flags				= 0;
		TesselationState.patchControlPoints = cp_count_;

		TesselationControlsStage	= tess_control_stage_;
		TesselationEvaluationStage	= tess_evaluation_stage_;
	}
};

typedef std::vector<VertexInputAttribute> InputAttributes;

struct StandartPipelineInfo
{
	VkPrimitiveTopology								PrimitiveTopology;
	Uint											ControlPointCount;

	InputAttributes									VertexStageInput;
	VkPipelineLayout								PipelineLayout;

    VkPipelineShaderStageCreateInfo					VertexStage;
	VkPipelineShaderStageCreateInfo					GeometricStage;
	TesselationStage								TesselationStage;
	VkPipelineShaderStageCreateInfo					FargmentStage;


    const VkPipelineRasterizationStateCreateInfo*   RasterizationState;
    const VkPipelineMultisampleStateCreateInfo*     MultisampleState;
    const VkPipelineDepthStencilStateCreateInfo*    DepthStencilState;
    const VkPipelineColorBlendStateCreateInfo*      ColorBlendState;

	VkAttachmentsFormats							ColorFormats;
	VkFormat										DepthFormat;
	CRenderPass*									ExistingRenderPass;//Optional use can also use existiens Render Pass instend create a new one from formats
};

class CPipeline
{
	private:
		std::string				m_Name;

		VkPipeline				p_Pipeline;
		CRenderPass*			p_RenderPass;

		VkDevice p_DeviceContext;

	public:
		Uint ComputeStride(InputAttributes attributes_);
		CPipeline(VkDevice device_, std::string pipeline_name_, StandartPipelineInfo pipeline_inf, VkBool32 present_);

		~CPipeline();

	CRenderPass* const GetRenderPass() { return p_RenderPass; }

	Void Bind(VkCommandBuffer command_buffer_)
	{
		vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, p_Pipeline);
	}
};