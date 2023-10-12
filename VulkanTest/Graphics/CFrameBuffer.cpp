#include "CFrameBuffer.h"

CFrameBuffer::CFrameBuffer(VkDevice device_, std::vector<CTextureBase*> color_targets_, CTextureBase* depth_target_, const CRenderPass* render_pass_)
	: p_Device(device_), p_RenderPass(render_pass_)
{
	this->m_ColorTargetsTextures	= color_targets_;
	this->p_DepthTargetTexture		= depth_target_;

	//Get dimantions...

	if (m_ColorTargetsTextures.size() > 0)
	{
		m_Width  = m_ColorTargetsTextures.at(0)->GetWidth();
		m_Height = m_ColorTargetsTextures.at(0)->GetHeight();
	}
	else if (depth_target_ != nullptr)
	{
		m_Width  = p_DepthTargetTexture->GetWidth();
		m_Height = p_DepthTargetTexture->GetHeight();
	}
	else
	{
		throw std::runtime_error("CFrameBuffer::CFrameBuffer -> Not targets found!");
		return;
	}


	if (!ChackTargetsMatching(color_targets_, depth_target_))
		return;

	CreateFrameBuffer();
}

Bool CFrameBuffer::ChackTargetsMatching(std::vector<CTextureBase*> color_targets_, CTextureBase* depth_target_)
{

	for (Uint i = 0; i < m_ColorTargetsTextures.size(); i++)
	{
		if (m_ColorTargetsTextures.at(i)->GetWidth() != m_Width || m_ColorTargetsTextures.at(i)->GetHeight() != m_Height)
		{
			throw std::runtime_error("CFrameBuffer::ChackTargetsMatching -> All render-targets/attachments must hace same dimantions!");
			return false;
		}

		if (m_ColorTargetsTextures.at(i)->GetFormat() != p_RenderPass->GetColorAttachmentsFormat(i))
		{
			throw std::runtime_error("CFrameBuffer::ChackTargetsMatching -> The format of color target number " + std::to_string(i) + " not matching to the one in the render pass!");
			return false;
		}
	}

	if (depth_target_ != nullptr)
	{
		if ((p_DepthTargetTexture->GetWidth() != m_Width || p_DepthTargetTexture->GetHeight() != m_Height))
		{
			throw std::runtime_error("CFrameBuffer::ChackTargetsMatching -> Depth target and color targets must have same dimantions");
			return false;
		}

		if (p_DepthTargetTexture->GetFormat() != p_RenderPass->GetDepthAttachmentFormat())
		{
			throw std::runtime_error("CFrameBuffer::ChackTargetsMatching -> The format of depth target not matching to the one in the render pass!");
			return false;
		}
	}
	else
	{
		if (p_RenderPass->GetDepthAttachmentFormat() != VK_FORMAT_UNDEFINED)
		{
			throw std::runtime_error("CFrameBuffer::ChackTargetsMatching -> The render pass use depth attachment but no depth buffer is used in this framebuffer!");
			return false;
		}
	}
}


Void CFrameBuffer::CreateFrameBuffer()
{
	Uint first_color_attachment = (p_DepthTargetTexture != nullptr) ? 1 : 0;
	std::vector<VkImageView> attachments(m_ColorTargetsTextures.size() + first_color_attachment);

	if (first_color_attachment)
		attachments.at(0) = p_DepthTargetTexture->GetBufferView();

	for (Uint i = 0; i < m_ColorTargetsTextures.size(); i++)
		attachments.at(i + first_color_attachment) = m_ColorTargetsTextures.at(i)->GetBufferView();

	VkFramebufferCreateInfo frame_buffer_desc {};
	frame_buffer_desc.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frame_buffer_desc.renderPass		= p_RenderPass->GetVkRenderPass();
	frame_buffer_desc.attachmentCount	= attachments.size();
	frame_buffer_desc.pAttachments		= attachments.data();
	frame_buffer_desc.width				= m_Width;
	frame_buffer_desc.height			= m_Height;
	frame_buffer_desc.layers			= p_RenderPass->GetLayersCount();

	if (vkCreateFramebuffer(p_Device, &frame_buffer_desc, nullptr, &p_FrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("CRenderPass::CreateFrameBuffer -> Failed to create framebuffer!");
		return;
	}
}


Void CFrameBuffer::Open(VkCommandBuffer command_buffer_, Vector4D clear_color_, Float clear_depth, Byte clear_stencil_)
{
	//Build clear values array...
	Uint color_attachment_start = (p_DepthTargetTexture != nullptr) ? 1 : 0;
	std::vector<VkClearValue> clear_values(m_ColorTargetsTextures.size() + color_attachment_start);

	if (p_DepthTargetTexture != nullptr)
	{
		VkClearValue depth_clear_value{};
		depth_clear_value.depthStencil.depth   = clear_depth;
		depth_clear_value.depthStencil.stencil = clear_stencil_;

		clear_values.at(0) = depth_clear_value;
	}
	
	VkClearValue color_clear_value{};
	memcpy(&color_clear_value.color, &clear_color_, sizeof(float) * 4);

	for (Uint i = 0 ; i < m_ColorTargetsTextures.size() ; i++)
		clear_values.at(i + color_attachment_start) = color_clear_value;

	//Begin render pass...
	VkRenderPassBeginInfo render_pass_begin_info{};
	render_pass_begin_info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass			= p_RenderPass->GetVkRenderPass();
	render_pass_begin_info.framebuffer			= p_FrameBuffer;
	render_pass_begin_info.renderArea.offset	= { 0, 0 };
	render_pass_begin_info.renderArea.extent	= {m_Width, m_Height};
	render_pass_begin_info.clearValueCount		= clear_values.size();
	render_pass_begin_info.pClearValues			= clear_values.data();
	
	vkCmdBeginRenderPass(command_buffer_, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

Void CFrameBuffer::Close(VkCommandBuffer command_buffer_)
{
	vkCmdEndRenderPass(command_buffer_);
}

//The next function bind all surfaces in same binding(As an descriptors array)
Void CFrameBuffer::BindSurfaces(VkDescriptorSet descriptor_set_, Uint binding_, VkSampler sampler_) const
{
	Uint color_attachment_start = (p_DepthTargetTexture != nullptr) ? 1 : 0;
	std::vector<VkDescriptorImageInfo> images_info(m_ColorTargetsTextures.size() + color_attachment_start);
	std::vector<VkWriteDescriptorSet>  descriptor_writes(m_ColorTargetsTextures.size() + color_attachment_start);
	
	if (color_attachment_start)
	{
		VkDescriptorImageInfo& image_info = images_info.at(0);
		image_info = {};
		image_info.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView	= p_DepthTargetTexture->GetBufferView();
		image_info.sampler		= sampler_;

		VkWriteDescriptorSet& descriptor_write = descriptor_writes.at(0);
		descriptor_write = {};
		descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet				= descriptor_set_;
		descriptor_write.dstBinding			= binding_;
		descriptor_write.dstArrayElement	= 0;
		descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write.descriptorCount	= 1;
		descriptor_write.pBufferInfo		= nullptr;
		descriptor_write.pImageInfo			= &image_info;
		descriptor_write.pTexelBufferView	= nullptr;
	}

	for (Uint i = color_attachment_start ; i < m_ColorTargetsTextures.size() + color_attachment_start ; i ++)
	{
		VkDescriptorImageInfo& image_info = images_info.at(i);
		image_info = {};
		image_info.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView	= m_ColorTargetsTextures.at(i - color_attachment_start)->GetBufferView();
		image_info.sampler		= sampler_;

		VkWriteDescriptorSet& descriptor_write = descriptor_writes.at(i);
		descriptor_write = {};
		descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet				= descriptor_set_;
		descriptor_write.dstBinding			= binding_ + i;
		descriptor_write.dstArrayElement	= 0;
		descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write.descriptorCount	= 1;
		descriptor_write.pBufferInfo		= nullptr;
		descriptor_write.pImageInfo			= &image_info;
		descriptor_write.pTexelBufferView	= nullptr;
	}

	vkUpdateDescriptorSets(p_Device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
}

Void CFrameBuffer::Barrier(VkCommandBuffer command_buffer_, BarrierState new_state_, Bool depth_translation_, Bool color_translation_)
{
	if (depth_translation_ && (p_DepthTargetTexture != nullptr))
	{
		VkImageMemoryBarrier depth_barrier = {};
		VkPipelineStageFlagBits depth_src_stage, depth_dst_stage;

		if (new_state_ == BarrierState::TARGET)
		{
			depth_barrier	= p_DepthTargetTexture->BarrierTranslation(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			depth_src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			depth_dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (new_state_ == BarrierState::RESOURCE)
		{
			depth_barrier	= p_DepthTargetTexture->BarrierTranslation(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			depth_src_stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depth_dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		if(depth_barrier.newLayout != depth_barrier.oldLayout)
			vkCmdPipelineBarrier(command_buffer_, depth_src_stage, depth_dst_stage, 0, 0, 0, 0, 0, 1, &depth_barrier);
	}
	
	if (color_translation_ && (m_ColorTargetsTextures.size() > 0))
	{
		VkImageLayout color_layout;
		std::vector<VkImageMemoryBarrier> color_barriers(m_ColorTargetsTextures.size());
		VkPipelineStageFlagBits color_src_stage, color_dst_stage;

		if (new_state_ == BarrierState::TARGET)
		{
			color_layout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			color_src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			color_dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (new_state_ == BarrierState::RESOURCE)
		{
			color_layout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			color_src_stage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			color_dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if(new_state_ == BarrierState::OUTPUT)
		{
			color_layout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			color_src_stage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			color_dst_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		Uint counter = 0;
		for (Uint i = 0; i < m_ColorTargetsTextures.size(); i++)
		{
			color_barriers.at(counter) = m_ColorTargetsTextures.at(i)->BarrierTranslation(color_layout);
				
			if (color_barriers.at(counter).newLayout != color_barriers.at(counter).oldLayout)
				counter ++;
		}

		vkCmdPipelineBarrier(command_buffer_, color_src_stage, color_dst_stage, 0, 0, 0, 0, 0, counter, color_barriers.data());
	}
}

Bool CFrameBuffer::IsBoundable()
{
	if (p_DepthTargetTexture != nullptr && !p_DepthTargetTexture->IsBoundable())
		return false;

	for (Uint i = 0; i < m_ColorTargetsTextures.size(); i++)
	{
		if (!m_ColorTargetsTextures.at(i)->IsBoundable())
			return false;
	}

	return true;
}

Bool CFrameBuffer::IsOutput()
{
	return (m_ColorTargetsTextures.size() == 1 && m_ColorTargetsTextures.at(0)->IsBaseOnBuffer());
}


CFrameBuffer::~CFrameBuffer()
{
	if(p_FrameBuffer != nullptr)
		vkDestroyFramebuffer(p_Device, p_FrameBuffer, nullptr);
}
