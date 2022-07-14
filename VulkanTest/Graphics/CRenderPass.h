#pragma once

#include "../Workspace.h"
#include "FormatsMap.h"

#define RENDER_PASS_MAXIMUM_ATTACHMENTS 8

typedef std::array<VkFormat, RENDER_PASS_MAXIMUM_ATTACHMENTS> VkAttachmentsFormats;

enum class RenderPassType
{
	Present,
	Texture2D,
	TextureCube
};

class CRenderPass
{
	friend class CPipeline;//CPipeline have write access

	private:
		VkRenderPass			p_RenderPass;

		VkAttachmentsFormats	m_ColorAttachmentsFormat;
		VkFormat				m_DepthAttachmentFormat;

		VkDevice p_DeviceContext;

		const RenderPassType m_TargetType;

	public:
		CRenderPass(VkDevice device_, VkAttachmentsFormats color_formats_, VkFormat depth_format_, RenderPassType type_, Bool color_autoclear_ = true, Bool depth_autoclear_ = true);


		~CRenderPass();

		VkFormat GetColorAttachmentsFormat(Uint index_) const { return m_ColorAttachmentsFormat[index_]; }
		VkFormat GetDepthAttachmentFormat() const { return m_DepthAttachmentFormat; }
		VkRenderPass GetVkRenderPass() const { return p_RenderPass; }
		Uint GetLayersCount() const { return (m_TargetType == RenderPassType::TextureCube) ? 6 : 1; }
};