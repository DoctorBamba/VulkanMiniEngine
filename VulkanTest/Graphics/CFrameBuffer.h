#pragma once
#include "CTexture2D.h"
#include "CTextureCube.h"
#include "CPipeline.h"



class CFrameBuffer
{
	public:
		enum class BarrierState
		{
			UNDEFINE,
			TARGET,
			RESOURCE,
			OUTPUT
		};

	private:
		CTextureBase*				p_DepthTargetTexture;
		std::vector<CTextureBase*>	m_ColorTargetsTextures;

		Uint16						m_Width;
		Uint16						m_Height;

		VkFramebuffer				p_FrameBuffer;
		const CRenderPass*			p_RenderPass;

		const VkDevice	 p_Device;

	private:
		Bool ChackTargetsMatching(std::vector<CTextureBase*> color_targets_, CTextureBase* depth_target_);
		Void CreateFrameBuffer();

	public:
		CFrameBuffer(VkDevice device_, std::vector<CTextureBase*> color_targets_, CTextureBase* depth_target_, const CRenderPass* render_pass_);
		virtual ~CFrameBuffer();

		Void Open(VkCommandBuffer command_buffer_, Vector4D clear_color_ = Vector4D(0.0f, 0.0f, 0.0f, 0.0f), Float clear_depth = 1.0f, Byte clear_stencil_ = 0);
		Void Close(VkCommandBuffer command_buffer_);

		CTextureBase* GetDepthStencilSurface() { return p_DepthTargetTexture; }
		CTextureBase* GetColorSurface(Uint inedx_) { return (inedx_ < m_ColorTargetsTextures.size()) ? m_ColorTargetsTextures.at(inedx_) : nullptr; }

		Void BindSurfaces(VkDescriptorSet descriptor_set_, Uint binding_, VkSampler sampler_) const;
		Void Barrier(VkCommandBuffer command_buffer_, BarrierState new_state_, Bool depth_translation_ = true, Bool color_translation_ = true);

		Uint GetColorAttachmentsCount() { return m_ColorTargetsTextures.size(); }

		Bool IsBoundable();
		Bool IsOutput();
};