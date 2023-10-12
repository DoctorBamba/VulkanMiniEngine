#include "CTextureCube.h"

CTextureCube::CTextureCube(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, VkFormat format_, Bool attachment_usage_,
						Pointer construct_data_, CGpuUploadTask* upload_task_, std::string name_) : CTextureBase(memory_space_, Dimension::Texture2DCube, CreationWay::CREATE_FROM_DATA)
{
	m_Name		= name_;
	m_Width		= width_;
	m_Height	= height_;
	m_Format	= format_;
	m_Layers	= 6;

	CreateBuffer(attachment_usage_ , construct_data_, upload_task_);
	CreateView();
}

Void CTextureCube::CreateView()
{
	VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

	if (IsDepthFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
	else if (IsColorFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;


	VkImageViewCreateInfo view_desc{};
	view_desc.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_desc.image								= p_Image;
	view_desc.viewType							= VK_IMAGE_VIEW_TYPE_CUBE;
	view_desc.format							= m_Format;
	view_desc.components.r						= VK_COMPONENT_SWIZZLE_R;
	view_desc.components.g						= VK_COMPONENT_SWIZZLE_G;
	view_desc.components.b						= VK_COMPONENT_SWIZZLE_B;
	view_desc.components.a						= VK_COMPONENT_SWIZZLE_A;
	view_desc.subresourceRange					= GetSubresourceRange();
	
	if (vkCreateImageView(p_Device->device, &view_desc, nullptr, &p_ImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("CTextureCube::CreateView -> Failed to create image view");
		return;
	}
}

Void CTextureCube::Upload(Pointer construct_data_, CGpuUploadTask* upload_task_)
{
	Uint layer_bytes = m_Width * m_Height * vk_format_table.at(m_Format).size;
	CIntermidiateBuffer* upload_buffer = new CIntermidiateBuffer(p_Device, layer_bytes * 6, CIntermidiateBuffer::Usage::Upload);
	
	Pointer* data_pointers = (Pointer*)construct_data_;
	for (Uint face = 0; face < 6; face ++)
		memcpy((Byte*)(upload_buffer->Map()) + face * layer_bytes, data_pointers[face], layer_bytes);

	upload_buffer->Unmap();

	std::vector<VkBufferImageCopy> copy_rigions(6);

	for(Uint face = 0 ; face < 6 ; face ++)
	{
		VkBufferImageCopy& region = copy_rigions.at(face);
		region = {};
		region.bufferOffset						= face * layer_bytes;
		region.bufferRowLength					= 0;
		region.bufferImageHeight				= 0;
		region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount		= 1;
		region.imageSubresource.baseArrayLayer	= face;
		region.imageSubresource.mipLevel		= 0;
		region.imageOffset						= {0, 0, 0};
		region.imageExtent						= {m_Width, m_Height, 1};
	}

	VkImageMemoryBarrier begin_barrier = BarrierTranslation(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdPipelineBarrier(upload_task_->GetCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &begin_barrier);
	
	vkCmdCopyBufferToImage(upload_task_->GetCommandBuffer(), upload_buffer->GetVkBuffer(), p_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copy_rigions.size(), copy_rigions.data());
	
	VkImageMemoryBarrier after_barrier = BarrierTranslation(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkCmdPipelineBarrier(upload_task_->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, 0, 0, 0, 1, &after_barrier);

	upload_task_->m_Intermidiates.push_back(upload_buffer);
}