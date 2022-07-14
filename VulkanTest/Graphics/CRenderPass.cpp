#include "CRenderPass.h"

CRenderPass::CRenderPass(VkDevice device_, VkAttachmentsFormats color_formats_, VkFormat depth_format_, RenderPassType type_ ,Bool color_autoclear_, Bool depth_autoclear_) : p_DeviceContext(device_), m_TargetType(type_)
{
	//Store attachments formats...
	m_ColorAttachmentsFormat = color_formats_;
	m_DepthAttachmentFormat	= depth_format_;

	//Define Color & Depth attachment...
	VkImageLayout color_image_layout = (type_ == RenderPassType::Present) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> all_attachments;
	VkAttachmentReference				depth_attachments_ref;
	std::vector<VkAttachmentReference>	color_attachments_ref;
	
	if(depth_format_ != VK_FORMAT_UNDEFINED)
	{ 
		VkAttachmentDescription depth_attachments;
		depth_attachments.flags				= 0;
		depth_attachments.format			= depth_format_;
		depth_attachments.samples			= VK_SAMPLE_COUNT_1_BIT;
		depth_attachments.loadOp			= depth_autoclear_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		depth_attachments.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		depth_attachments.stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachments.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachments.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachments.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		all_attachments.push_back(depth_attachments);

		depth_attachments_ref.attachment	= 0;
		depth_attachments_ref.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	Uint color_attachments_start_index = all_attachments.size();

	for (Uint i = 0; i < RENDER_PASS_MAXIMUM_ATTACHMENTS; i++)
	{
		if (color_formats_[i] == VK_FORMAT_UNDEFINED)
			continue;

		VkAttachmentDescription color_attachment;
		color_attachment.flags			= 0;
		color_attachment.format			= color_formats_[i];
		color_attachment.samples		= VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp			= color_autoclear_ ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		color_attachment.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout	= color_image_layout;
		all_attachments.push_back(color_attachment);

		VkAttachmentReference color_attachment_ref;
		color_attachment_ref.attachment = color_attachments_start_index + i;
		color_attachment_ref.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_attachments_ref.push_back(color_attachment_ref);
	}
	
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount		= color_attachments_ref.size();
	subpass.pColorAttachments			= color_attachments_ref.data();
	subpass.pDepthStencilAttachment		= (depth_format_ != VK_FORMAT_UNDEFINED) ? &depth_attachments_ref : nullptr;

	VkRenderPassCreateInfo render_pass_desc{};
	render_pass_desc.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_desc.attachmentCount	= all_attachments.size();
	render_pass_desc.pAttachments		= all_attachments.data();
	render_pass_desc.subpassCount		= 1;
	render_pass_desc.pSubpasses			= &subpass;

	if (type_ == RenderPassType::Present)
	{
		VkSubpassDependency dependency{};
		dependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass		= 0;
		dependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask	= 0;
		dependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		render_pass_desc.dependencyCount = 1;
		render_pass_desc.pDependencies	 = &dependency;
	}

	if (vkCreateRenderPass(p_DeviceContext, &render_pass_desc, nullptr, &p_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("CRenderPass::CRenderPass -> Failed to create render pass!");
		return;
	}
}


CRenderPass::~CRenderPass()
{
	if (p_RenderPass != nullptr)
		vkDestroyRenderPass(p_DeviceContext, p_RenderPass, nullptr);
}