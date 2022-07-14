#pragma once
#include "../../Workspace.h"

/*Blend States...*/

#define MAXIMUN_ATTACHMENT_PER_RENDERPASS 8

static VkPipelineColorBlendAttachmentState VulkanUsefuls_ColorBlendAttachmentState_Writeover =
{
	VK_FALSE,								//blendEnable
	VK_BLEND_FACTOR_ONE,					//srcColorBlendFactor
	VK_BLEND_FACTOR_ZERO,					//dstColorBlendFactor
	VK_BLEND_OP_ADD,						//colorBlendOp
	VK_BLEND_FACTOR_ONE,					//srcAlphaBlendFactor
	VK_BLEND_FACTOR_ZERO,					//dstAlphaBlendFactor
	VK_BLEND_OP_ADD,						//alphaBlendOp
	(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)//colorWriteMask
};

static VkPipelineColorBlendAttachmentState VulkanUsefuls_ColorBlendAttachmentState_Regular =
{
	VK_TRUE,								//blendEnable
	VK_BLEND_FACTOR_SRC_ALPHA,				//srcColorBlendFactor
	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,	//dstColorBlendFactor
	VK_BLEND_OP_ADD,						//colorBlendOp
	VK_BLEND_FACTOR_ONE,					//srcAlphaBlendFactor
	VK_BLEND_FACTOR_ONE,					//dstAlphaBlendFactor
	VK_BLEND_OP_ADD,						//alphaBlendOp
	(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)//colorWriteMask
};

static VkPipelineColorBlendAttachmentState VulkanUsefuls_ColorBlendAttachmentState_Add =
{
	VK_TRUE,								//blendEnable
	VK_BLEND_FACTOR_ONE,					//srcColorBlendFactor
	VK_BLEND_FACTOR_ONE,					//dstColorBlendFactor
	VK_BLEND_OP_ADD,						//colorBlendOp
	VK_BLEND_FACTOR_ONE,					//srcAlphaBlendFactor
	VK_BLEND_FACTOR_ONE,					//dstAlphaBlendFactor
	VK_BLEND_OP_ADD,						//alphaBlendOp
	(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)//colorWriteMask
};


struct VulkanUsefuls_BlendState_Writeover__ {};
extern const __declspec(selectany) VulkanUsefuls_BlendState_Writeover__ VULKAN_USEFULS_BLEND_STATE_WRITEOVER;

struct VulkanUsefuls_BlendState_Regular__ {};
extern const __declspec(selectany) VulkanUsefuls_BlendState_Regular__ VULKAN_USEFULS_BLEND_STATE_REGULAR;

struct VulkanUsefuls_BlendState_Add__ {};
extern const __declspec(selectany) VulkanUsefuls_BlendState_Add__ VULKAN_USEFULS_BLEND_STATE_ADD;

struct VulkanUsefuls_BlendState : public VkPipelineColorBlendStateCreateInfo
{
	VulkanUsefuls_BlendState() = default;
	explicit VulkanUsefuls_BlendState(const VkPipelineColorBlendStateCreateInfo& blend_state_) noexcept :
		VkPipelineColorBlendStateCreateInfo(blend_state_) {}

	explicit VulkanUsefuls_BlendState(VulkanUsefuls_BlendState_Writeover__, Uint attachment_count_) noexcept
	{
		VkPipelineColorBlendAttachmentState* attachments = new VkPipelineColorBlendAttachmentState[attachment_count_];
		for (Uint i = 0; i < attachment_count_; i++)
			attachments[i] = VulkanUsefuls_ColorBlendAttachmentState_Writeover;

		sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		flags				= 0;
		pNext				= nullptr;

		logicOpEnable		= VK_FALSE;
		logicOp				= VK_LOGIC_OP_COPY;
		attachmentCount		= attachment_count_;
		pAttachments		= attachments;
		blendConstants[0]	= 0.0f;
		blendConstants[1]	= 0.0f;
		blendConstants[2]	= 0.0f;
		blendConstants[3]	= 0.0f;
	}

    explicit VulkanUsefuls_BlendState(VulkanUsefuls_BlendState_Regular__) noexcept
	{
		sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		flags				= 0;
		pNext				= nullptr;

		logicOpEnable		= VK_FALSE;
		logicOp				= VK_LOGIC_OP_COPY;
		attachmentCount		= 1;
		pAttachments		= &VulkanUsefuls_ColorBlendAttachmentState_Regular;
		blendConstants[0]	= 0.0f;
		blendConstants[1]	= 0.0f;
		blendConstants[2]	= 0.0f;
		blendConstants[3]	= 0.0f;
	}

	explicit VulkanUsefuls_BlendState(VulkanUsefuls_BlendState_Add__) noexcept
	{
		sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		flags				= 0;
		pNext				= nullptr;

		logicOpEnable		= VK_FALSE;
		logicOp				= VK_LOGIC_OP_COPY;
		attachmentCount		= 1;
		pAttachments		= &VulkanUsefuls_ColorBlendAttachmentState_Add;
		blendConstants[0]	= 0.0f;
		blendConstants[1]	= 0.0f;
		blendConstants[2]	= 0.0f;
		blendConstants[3]	= 0.0f;
	}
};

/*Depth Stancil State*/

struct VulkanUsefuls_DepthStancilState_Regular__ {};
extern const __declspec(selectany) VulkanUsefuls_DepthStancilState_Regular__ VULKAN_USEFULS_DEPTH_STANCIL_STATE_LESS;

struct VulkanUsefuls_DepthStancilState_Disable__ {};
extern const __declspec(selectany) VulkanUsefuls_DepthStancilState_Disable__ VULKAN_USEFULS_DEPTH_STANCIL_STATE_DISABLE;


struct VulkanUsefuls_DepthStancilState : public VkPipelineDepthStencilStateCreateInfo
{
	VulkanUsefuls_DepthStancilState() = default;
	explicit VulkanUsefuls_DepthStancilState(const VkPipelineDepthStencilStateCreateInfo& depth_stancil_state_) noexcept :
		VkPipelineDepthStencilStateCreateInfo(depth_stancil_state_) {}

    explicit VulkanUsefuls_DepthStancilState(VulkanUsefuls_DepthStancilState_Regular__, Bool write_ = true, Bool mask_ = true) noexcept
	{
		sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		flags					= 0;
		pNext					= nullptr;

		depthWriteEnable		= write_;
		depthTestEnable			= mask_;
		depthCompareOp			= VK_COMPARE_OP_LESS;
		depthBoundsTestEnable	= VK_FALSE;
		minDepthBounds			= 0.0f;
		maxDepthBounds			= 1.0f;
		stencilTestEnable		= VK_FALSE;
		front					= {};
		back					= {};
	}

	explicit VulkanUsefuls_DepthStancilState(VulkanUsefuls_DepthStancilState_Disable__) noexcept
	{
		sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		flags					= 0;
		pNext					= nullptr;

		depthTestEnable			= VK_FALSE;
		depthWriteEnable		= VK_FALSE;
		depthCompareOp			= VK_COMPARE_OP_LESS;
		depthBoundsTestEnable	= VK_FALSE;
		minDepthBounds			= 0.0f;
		maxDepthBounds			= 1.0f;
		stencilTestEnable		= VK_FALSE;
		front					= {};
		back					= {};
	}
};

/*Rasterizer States...*/

struct VulkanUsefuls_RasterizerState_Fill__ {};
extern const __declspec(selectany) VulkanUsefuls_RasterizerState_Fill__ VULKAN_USEFULS_RASTERIZER_STATE_FILL;

struct VulkanUsefuls_RasterizerState_Solid_Wireframe_ {};
extern const __declspec(selectany) VulkanUsefuls_RasterizerState_Solid_Wireframe_ VULKAN_USEFULS_RASTERIZER_STATE_WIREFRAME;

struct VulkanUsefuls_RasterizerState : public VkPipelineRasterizationStateCreateInfo
{
	VulkanUsefuls_RasterizerState() = default;
	explicit VulkanUsefuls_RasterizerState(const VkPipelineRasterizationStateCreateInfo& rasterization_state_) noexcept :
		VkPipelineRasterizationStateCreateInfo(rasterization_state_) {}

    explicit VulkanUsefuls_RasterizerState(VulkanUsefuls_RasterizerState_Fill__, VkCullModeFlags call_mode_ = VK_CULL_MODE_FRONT_BIT) noexcept
	{
		sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		flags					= 0;
		pNext					= nullptr;

		depthClampEnable		= VK_FALSE;
		rasterizerDiscardEnable = VK_FALSE;
		polygonMode				= VK_POLYGON_MODE_FILL;
		lineWidth				= 3.0f;
		cullMode				= call_mode_;
		frontFace				= VK_FRONT_FACE_CLOCKWISE;
		depthBiasEnable			= VK_FALSE;
		depthBiasConstantFactor = 0.0f;
		depthBiasClamp			= 0.0f;
		depthBiasSlopeFactor	= 0.0f;
	}

	explicit VulkanUsefuls_RasterizerState(VulkanUsefuls_RasterizerState_Solid_Wireframe_, float line_width_ = 1.0) noexcept
	{
		sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		flags					= 0;
		pNext					= nullptr;

		depthClampEnable		= VK_FALSE;
		rasterizerDiscardEnable = VK_FALSE;
		polygonMode				= VK_POLYGON_MODE_LINE;
		lineWidth				= line_width_;
		cullMode				= VK_CULL_MODE_NONE;
		frontFace				= VK_FRONT_FACE_CLOCKWISE;
		depthBiasEnable			= VK_FALSE;
		depthBiasConstantFactor = 0.0f;
		depthBiasClamp			= 0.0f;
		depthBiasSlopeFactor	= 0.0f;
	}
};

/*Multisample States...*/

struct VulkanUsefuls_MultisampleState_Default__ {};
extern const __declspec(selectany) VulkanUsefuls_MultisampleState_Default__ VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT;

struct VulkanUsefuls_MultisamplerState : public VkPipelineMultisampleStateCreateInfo
{
	VulkanUsefuls_MultisamplerState() = default;
	explicit VulkanUsefuls_MultisamplerState(const VkPipelineMultisampleStateCreateInfo& ms_state_) noexcept :
		VkPipelineMultisampleStateCreateInfo(ms_state_) {}

    explicit VulkanUsefuls_MultisamplerState(VulkanUsefuls_MultisampleState_Default__) noexcept
	{
		sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		flags					= 0;
		pNext					= nullptr;

		sampleShadingEnable		= VK_FALSE;
		rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
		minSampleShading		= 1.0f;
		pSampleMask				= nullptr;
		alphaToCoverageEnable	= VK_FALSE;
		alphaToOneEnable		= VK_FALSE;
	}
};
