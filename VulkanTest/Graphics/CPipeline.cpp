#include "CPipeline.h"


Uint CPipeline::ComputeStride(InputAttributes attributes_)
{
	Uint stride = 0;

	for (Uint i = 0; i < attributes_.size(); i++)
		stride += vk_format_table.at(attributes_.at(i).format).size;

	return stride;
}

CPipeline::CPipeline(VkDevice device_, std::string pipeline_name_, StandartPipelineInfo pipeline_desc_, VkBool32 present_) 
	: p_DeviceContext(device_), m_Name(pipeline_name_)
{
	//Dynamic State...
	VkDynamicState dynamic_states[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{};
	dynamic_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = array_size(dynamic_states);
	dynamic_state.pDynamicStates	= dynamic_states;

	//Input assembly...
	VkPipelineInputAssemblyStateCreateInfo input_assembly_desc{};
	input_assembly_desc.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_desc.topology				= pipeline_desc_.PrimitiveTopology;
	input_assembly_desc.primitiveRestartEnable	= VK_FALSE;

	//Vertex Stage inputs...

	Uint binding_count = 0;
	VkVertexInputBindingDescription binding_descriptions[8];
	std::vector<VkVertexInputAttributeDescription> attributes(pipeline_desc_.VertexStageInput.size());
	std::map<Uint, Uint> binding_map;

	for (Uint i = 0; i < pipeline_desc_.VertexStageInput.size(); i++)
	{
		const VertexInputAttribute& input_attr = pipeline_desc_.VertexStageInput.at(i);
		const Uint format_size = vk_format_table.at(input_attr.format).size;


		auto found = binding_map.find(input_attr.binding);
		if (found == binding_map.end())
		{
			binding_descriptions[binding_count].binding		= input_attr.binding;
			binding_descriptions[binding_count].inputRate	= input_attr.input_rate;

			binding_count++;

			attributes[i].location	= input_attr.location;
			attributes[i].binding	= input_attr.binding;
			attributes[i].format	= input_attr.format;
			attributes[i].offset	= 0;

			binding_map.insert(std::pair<Uint, Uint>(input_attr.binding, format_size));
		}
		else
		{
			attributes[i].location	= input_attr.location;
			attributes[i].binding	= input_attr.binding;
			attributes[i].format	= input_attr.format;
			attributes[i].offset	= found->second;

			found->second += format_size;
		}
	}

	for (Uint i = 0; i < binding_count; i++)
		binding_descriptions[i].stride = binding_map.at(binding_descriptions[i].binding);
	

	VkPipelineVertexInputStateCreateInfo vertex_input_desc{};
	vertex_input_desc.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_desc.vertexBindingDescriptionCount		= binding_count;
	vertex_input_desc.pVertexBindingDescriptions		= binding_descriptions; // Optional
	vertex_input_desc.vertexAttributeDescriptionCount	= attributes.size();
	vertex_input_desc.pVertexAttributeDescriptions		= (attributes.size() > 0) ? attributes.data() : nullptr; // Optional

	//Pipelines stages...
	Bool useing_tesselation = false;
	std::vector<VkPipelineShaderStageCreateInfo> stages;

	if (pipeline_desc_.VertexStage.stage == VK_SHADER_STAGE_VERTEX_BIT)
		stages.push_back(pipeline_desc_.VertexStage);
	else
		throw std::runtime_error("CPipeline::CPipeline -> The Vertex Stage is undefined!");

	if (pipeline_desc_.FargmentStage.stage == VK_SHADER_STAGE_FRAGMENT_BIT)
		stages.push_back(pipeline_desc_.FargmentStage);

	if (pipeline_desc_.GeometricStage.stage == VK_SHADER_STAGE_GEOMETRY_BIT)
		stages.push_back(pipeline_desc_.GeometricStage);

	if (pipeline_desc_.PrimitiveTopology ==	VK_PRIMITIVE_TOPOLOGY_PATCH_LIST)
	{
		useing_tesselation = true;

		if (pipeline_desc_.TesselationStage.TesselationControlsStage.module == VK_NULL_HANDLE)
			throw std::runtime_error("CPipeline::CPipeline -> Pipeline's topology requierd Tesselation-Control-Stage but this stage was not provided!");
		else if(pipeline_desc_.TesselationStage.TesselationControlsStage.stage != VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
			throw std::runtime_error("CPipeline::CPipeline -> The given Tesselation-Controls-Stage module was not declared with the 'VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT' flag!");
		if (pipeline_desc_.TesselationStage.TesselationEvaluationStage.module == VK_NULL_HANDLE)
			throw std::runtime_error("CPipeline::CPipeline -> Pipeline's topology requierd Tesselation-Evaluation-Stage but this stage was not provided!");
		else if (pipeline_desc_.TesselationStage.TesselationEvaluationStage.stage != VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
			throw std::runtime_error("CPipeline::CPipeline -> The given Tesselation-Evaluation-Stage module was not declared with the 'VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT' flag!");
		if(pipeline_desc_.TesselationStage.TesselationState.patchControlPoints < 3)
			throw std::runtime_error("CPipeline::CPipeline -> Controls Points count must be larger or eual to 3!");
	
		stages.push_back(pipeline_desc_.TesselationStage.TesselationControlsStage);
		stages.push_back(pipeline_desc_.TesselationStage.TesselationEvaluationStage);
	}

	//Initional Viewport State...
	VkViewport initional_viewport{};
	initional_viewport.x			= 0;
	initional_viewport.y			= 0;
	initional_viewport.width		= 1024;
	initional_viewport.height		= 768;
	initional_viewport.minDepth		= 0.0f;
	initional_viewport.maxDepth		= 1.0f;

	VkRect2D initional_scissor{};
	initional_scissor.offset = { 0, 0 };
	initional_scissor.extent = { (Uint)1024, (Uint)768 };

	VkPipelineViewportStateCreateInfo initional_viewport_state_desc{};
	initional_viewport_state_desc.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	initional_viewport_state_desc.viewportCount	= 1;
	initional_viewport_state_desc.pViewports	= &initional_viewport;
	initional_viewport_state_desc.scissorCount	= 1;
	initional_viewport_state_desc.pScissors		= &initional_scissor;

	//Render Pass...
	if (pipeline_desc_.ExistingRenderPass != nullptr)
		p_RenderPass = pipeline_desc_.ExistingRenderPass;
	else//Else Create a new one acoording to the given formats
		p_RenderPass = new CRenderPass(p_DeviceContext, pipeline_desc_.ColorFormats, pipeline_desc_.DepthFormat, present_ ? RenderPassType::Present : RenderPassType::Texture2D);

	//Create The Pipeline...

	VkGraphicsPipelineCreateInfo pipeline_desc{};

	pipeline_desc.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_desc.layout				= pipeline_desc_.PipelineLayout;
	pipeline_desc.stageCount			= stages.size();
	pipeline_desc.pStages				= stages.data();
	pipeline_desc.pVertexInputState		= &vertex_input_desc;
	pipeline_desc.pInputAssemblyState	= &input_assembly_desc;
	pipeline_desc.pTessellationState	= (useing_tesselation) ? &pipeline_desc_.TesselationStage.TesselationState : nullptr;
	pipeline_desc.pRasterizationState	= pipeline_desc_.RasterizationState;
	pipeline_desc.pMultisampleState		= pipeline_desc_.MultisampleState;
	pipeline_desc.pDepthStencilState	= pipeline_desc_.DepthStencilState;
	pipeline_desc.pColorBlendState		= pipeline_desc_.ColorBlendState;
	pipeline_desc.pViewportState		= &initional_viewport_state_desc;
	pipeline_desc.pDynamicState			= &dynamic_state;
	pipeline_desc.renderPass			= p_RenderPass->p_RenderPass;
	pipeline_desc.subpass				= 0;
	pipeline_desc.basePipelineHandle	= VK_NULL_HANDLE;
	pipeline_desc.basePipelineIndex		= -1;

	if (vkCreateGraphicsPipelines(p_DeviceContext, VK_NULL_HANDLE, 1, &pipeline_desc, nullptr, &p_Pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("CreateNewPipelineState -> Failed to create '" + m_Name + "' graphics pipeline!");
		return;
	}
}

CPipeline::~CPipeline()
{
	if (p_Pipeline != nullptr)
		vkDestroyPipeline(p_DeviceContext, p_Pipeline, nullptr);

	delete p_RenderPass;
}