#include "CPipeline.h"
#include "../Engine/Deafults/VulkanDefaultStates.h"

Uint CPipeline::ComputeStride(InputAttributes attributes_)
{
	Uint stride = 0;

	for (Uint i = 0; i < attributes_.size(); i++)
		stride += vk_format_table.at(attributes_.at(i).format).size;

	return stride;
}

static std::unordered_map<PrimitiveTopology, VkPrimitiveTopology> PrimitiveTopologyVkTable
{
	{PrimitiveTopology::Points, VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
	{PrimitiveTopology::Lines, VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
	{PrimitiveTopology::Triangles, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
	{PrimitiveTopology::Quads, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP},
	{PrimitiveTopology::PatchList, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST}
};

static std::unordered_map<FaceCulling, VkCullModeFlagBits> FaceCullingVkTable
{
	{FaceCulling::None, VK_CULL_MODE_NONE},
	{FaceCulling::Front, VK_CULL_MODE_FRONT_BIT},
	{FaceCulling::Back, VK_CULL_MODE_BACK_BIT},
	{FaceCulling::FrontAndBack, VK_CULL_MODE_FRONT_AND_BACK},
};

static std::unordered_map<PolygonMode, VkPolygonMode> PolygonModeVkTable
{
	{PolygonMode::Fill, VK_POLYGON_MODE_FILL},
	{PolygonMode::Line, VK_POLYGON_MODE_LINE},
	{PolygonMode::Point, VK_POLYGON_MODE_POINT}
};

static std::unordered_map<AttachmentBlendingState::BlendingOperator, VkBlendOp> BlendingOperatorVkTable
{
	{AttachmentBlendingState::BlendingOperator::Add, VK_BLEND_OP_ADD},
	{AttachmentBlendingState::BlendingOperator::Subtract, VK_BLEND_OP_SUBTRACT}
};

static std::unordered_map<AttachmentBlendingState::BlendingFactor, VkBlendFactor> BlendingFactorVkTable
{
	{AttachmentBlendingState::BlendingFactor::Zero, VK_BLEND_FACTOR_ZERO},
	{AttachmentBlendingState::BlendingFactor::One, VK_BLEND_FACTOR_ONE},
	{AttachmentBlendingState::BlendingFactor::SrcAlpha, VK_BLEND_FACTOR_SRC_ALPHA},
	{AttachmentBlendingState::BlendingFactor::DstAlpha, VK_BLEND_FACTOR_DST_ALPHA},
	{AttachmentBlendingState::BlendingFactor::OneMinusSrcAlpha, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
	{AttachmentBlendingState::BlendingFactor::OneMinusDestAlpha, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA}
};

static std::unordered_map<ComparisonOperator, VkCompareOp> ComparisionOperatorVkTable
{
	{ComparisonOperator::Never, VK_COMPARE_OP_NEVER},
	{ComparisonOperator::Allway, VK_COMPARE_OP_ALWAYS},
	{ComparisonOperator::Less, VK_COMPARE_OP_LESS},
	{ComparisonOperator::Greater, VK_COMPARE_OP_GREATER},
	{ComparisonOperator::Equal, VK_COMPARE_OP_EQUAL},
	{ComparisonOperator::NotEqual, VK_COMPARE_OP_NOT_EQUAL}
};


CPipeline::CPipeline(VkDevice device_, std::string pipeline_name_, PipelineState pipeline_state_, VkBool32 present_) 
	: p_Device(device_), m_Name(pipeline_name_)
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
	input_assembly_desc.topology				= PrimitiveTopologyVkTable[pipeline_state_.input.topology];
	input_assembly_desc.primitiveRestartEnable	= VK_FALSE;

	//Vertex Stage inputs...

	Uint binding_count = 0;
	VkVertexInputBindingDescription binding_descriptions[8];
	std::vector<VkVertexInputAttributeDescription> attributes(pipeline_state_.input.vertexAttributes.size());
	std::map<Uint, Uint> binding_map;

	for (Uint i = 0; i < pipeline_state_.input.vertexAttributes.size() ; i++)
	{
		const VertexInputAttribute& input_attr = pipeline_state_.input.vertexAttributes.at(i);
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

	if (pipeline_state_.vertexStage.stage == VK_SHADER_STAGE_VERTEX_BIT)
		stages.push_back(pipeline_state_.vertexStage);
	else
		throw std::runtime_error("CPipeline::CPipeline -> The Vertex Stage is undefined!");

	if (pipeline_state_.fargmentStage.stage == VK_SHADER_STAGE_FRAGMENT_BIT)
		stages.push_back(pipeline_state_.fargmentStage);

	if (pipeline_state_.geometricStage.stage == VK_SHADER_STAGE_GEOMETRY_BIT)
		stages.push_back(pipeline_state_.geometricStage);

	if (pipeline_state_.input.topology == PrimitiveTopology::PatchList)
	{
		useing_tesselation = true;

		if (pipeline_state_.tesselationStage.TesselationControlsStage.module == VK_NULL_HANDLE)
			throw std::runtime_error("CPipeline::CPipeline -> Pipeline's topology requierd Tesselation-Control-Stage but this stage was not provided!");
		else if(pipeline_state_.tesselationStage.TesselationControlsStage.stage != VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
			throw std::runtime_error("CPipeline::CPipeline -> The given Tesselation-Controls-Stage module was not declared with the 'VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT' flag!");
		if (pipeline_state_.tesselationStage.TesselationEvaluationStage.module == VK_NULL_HANDLE)
			throw std::runtime_error("CPipeline::CPipeline -> Pipeline's topology requierd Tesselation-Evaluation-Stage but this stage was not provided!");
		else if (pipeline_state_.tesselationStage.TesselationEvaluationStage.stage != VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
			throw std::runtime_error("CPipeline::CPipeline -> The given Tesselation-Evaluation-Stage module was not declared with the 'VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT' flag!");
		if(pipeline_state_.tesselationStage.TesselationState.patchControlPoints < 3)
			throw std::runtime_error("CPipeline::CPipeline -> Controls Points count must be larger or eual to 3!");
	
		stages.push_back(pipeline_state_.tesselationStage.TesselationControlsStage);
		stages.push_back(pipeline_state_.tesselationStage.TesselationEvaluationStage);
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
	if (pipeline_state_.output.existingRenderPass != nullptr)
		p_RenderPass = pipeline_state_.output.existingRenderPass;
	else//Else Create a new one acoording to the given formats
		p_RenderPass = new CRenderPass(p_Device, pipeline_state_.output.colorFormats, pipeline_state_.output.depthFormat, present_ ? RenderPassType::Present : RenderPassType::Texture2D);

	
	
	//Rasterization State...

	VkPipelineRasterizationStateCreateInfo vk_rasterization_state = {};
	vk_rasterization_state.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vk_rasterization_state.flags					= 0;
	vk_rasterization_state.pNext					= nullptr;

	vk_rasterization_state.depthClampEnable			= VK_FALSE;
	vk_rasterization_state.rasterizerDiscardEnable	= VK_FALSE;
	vk_rasterization_state.polygonMode				= PolygonModeVkTable[pipeline_state_.rasterization.polygonMode];
	vk_rasterization_state.lineWidth				= 3.0f;
	vk_rasterization_state.cullMode					= FaceCullingVkTable[pipeline_state_.rasterization.faceCulling];
	vk_rasterization_state.frontFace				= VK_FRONT_FACE_CLOCKWISE;
	vk_rasterization_state.depthBiasEnable			= VK_FALSE;
	vk_rasterization_state.depthBiasConstantFactor	= 0.0f;
	vk_rasterization_state.depthBiasClamp			= 0.0f;
	vk_rasterization_state.depthBiasSlopeFactor		= 0.0f;

	//Color Attachntments Blending State...

	Uint attachntmentCount = pipeline_state_.output.GetColorAttachntmentCount();
	VkPipelineColorBlendAttachmentState* attachments = new VkPipelineColorBlendAttachmentState[attachntmentCount];
	
	for (int a = 0; a < attachntmentCount; a++)
	{
		const AttachmentBlendingState& attachmentblendingState = pipeline_state_.output.blendingStates[a];

		attachments[a] =
		{
			attachmentblendingState.IsBlendingEnabled() ? VK_TRUE : VK_FALSE,	//blendEnable
			BlendingFactorVkTable[attachmentblendingState.srcFactor],			//srcColorBlendFactor
			BlendingFactorVkTable[attachmentblendingState.dstFactor],			//dstColorBlendFactor
			BlendingOperatorVkTable[attachmentblendingState.blendeingOperator],	//colorBlendOp
			BlendingFactorVkTable[attachmentblendingState.srcFactor],			//srcAlphaBlendFactor
			BlendingFactorVkTable[attachmentblendingState.dstFactor],			//dstAlphaBlendFactor
			BlendingOperatorVkTable[attachmentblendingState.blendeingOperator],	//alphaBlendOp
			(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)//colorWriteMask
		};
	}

	VkPipelineColorBlendStateCreateInfo vk_blendings_state = {};
	vk_blendings_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vk_blendings_state.flags				= 0;
	vk_blendings_state.pNext				= nullptr;

	vk_blendings_state.logicOpEnable		= VK_FALSE;
	vk_blendings_state.logicOp				= VK_LOGIC_OP_COPY;
	vk_blendings_state.attachmentCount		= attachntmentCount;
	vk_blendings_state.pAttachments			= attachments;
	vk_blendings_state.blendConstants[0]	= 0.0f;
	vk_blendings_state.blendConstants[1]	= 0.0f;
	vk_blendings_state.blendConstants[2]	= 0.0f;
	vk_blendings_state.blendConstants[3]	= 0.0f;


	//Depth State...

	VkPipelineDepthStencilStateCreateInfo vs_depth_state = {};
	vs_depth_state.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	vs_depth_state.flags					= 0;
	vs_depth_state.pNext					= nullptr;

	vs_depth_state.depthWriteEnable			= pipeline_state_.output.depthState.writeDepth ? VK_TRUE : VK_FALSE;
	vs_depth_state.depthTestEnable			= pipeline_state_.output.depthState.maskDepth ? VK_TRUE : VK_FALSE;
	vs_depth_state.depthCompareOp			= ComparisionOperatorVkTable[pipeline_state_.output.depthState.testingOperator];
	vs_depth_state.depthBoundsTestEnable	= VK_FALSE;
	vs_depth_state.minDepthBounds			= 0.0f;
	vs_depth_state.maxDepthBounds			= 1.0f;
	vs_depth_state.stencilTestEnable		= VK_FALSE;
	vs_depth_state.front					= {};
	vs_depth_state.back						= {};


	//Create The Pipeline...

	VkGraphicsPipelineCreateInfo pipeline_desc{};

	pipeline_desc.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_desc.layout				= pipeline_state_.input.inputLayout;
	pipeline_desc.stageCount			= stages.size();
	pipeline_desc.pStages				= stages.data();
	pipeline_desc.pVertexInputState		= &vertex_input_desc;
	pipeline_desc.pInputAssemblyState	= &input_assembly_desc;
	pipeline_desc.pTessellationState	= (useing_tesselation) ? &pipeline_state_.tesselationStage.TesselationState : nullptr;
	pipeline_desc.pRasterizationState	= &vk_rasterization_state;
	pipeline_desc.pMultisampleState		= &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
	pipeline_desc.pDepthStencilState	= &vs_depth_state;
	pipeline_desc.pColorBlendState		= &vk_blendings_state;
	pipeline_desc.pViewportState		= &initional_viewport_state_desc;
	pipeline_desc.pDynamicState			= &dynamic_state;
	pipeline_desc.renderPass			= p_RenderPass->p_RenderPass;
	pipeline_desc.subpass				= 0;
	pipeline_desc.basePipelineHandle	= VK_NULL_HANDLE;
	pipeline_desc.basePipelineIndex		= -1;

	if (vkCreateGraphicsPipelines(p_Device, VK_NULL_HANDLE, 1, &pipeline_desc, nullptr, &p_Pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("CreateNewPipelineState -> Failed to create '" + m_Name + "' graphics pipeline!");
		return;
	}
}

CPipeline::~CPipeline()
{
	if (p_Pipeline != nullptr)
		vkDestroyPipeline(p_Device, p_Pipeline, nullptr);

	delete p_RenderPass;
}