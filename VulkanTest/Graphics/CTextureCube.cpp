#include "CTextureCube.h"

CTextureCube::CTextureCube(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, VkFormat format_, Bool attachment_usage_,
						Pointer construct_data_, CGpuUploadTask* upload_task_, std::string name_) : CTextureBase(memory_space_, CreationWay::CREATE_FROM_DATA)
{
	m_Name		= name_;
	m_Width		= width_;
	m_Height	= height_;
	m_Format	= format_;
	
	CreateBuffer(attachment_usage_, construct_data_, upload_task_);
	CreateView();
}

VkImageSubresourceRange CTextureCube::GetSubresourceRange()
{
	VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

	if (IsDepthFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
	else if (IsColorFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageSubresourceRange subresource_range;
	subresource_range.aspectMask		= aspect_flags;
	subresource_range.baseMipLevel		= 0;
	subresource_range.levelCount		= 1;
	subresource_range.baseArrayLayer	= 0;
	subresource_range.layerCount		= 6;

	return subresource_range;
}

Void CTextureCube::CreateBuffer(Bool attachment_usage_, Pointer construct_data_, CGpuUploadTask* upload_task_)
{
	VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	if (IsDepthFormat(m_Format))
	{
		if (attachment_usage_)
			usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	else if (IsColorFormat(m_Format))
	{
		if (attachment_usage_)
			usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}

	VkImageCreateInfo image_desc{};
	image_desc.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	image_desc.imageType		= VK_IMAGE_TYPE_2D;
	image_desc.format			= m_Format;
	image_desc.extent.width		= m_Width;
	image_desc.extent.height	= m_Height;
	image_desc.extent.depth		= 1;
	image_desc.mipLevels		= 1;
	image_desc.arrayLayers		= 6;
	image_desc.usage			= usage;
	image_desc.tiling			= VK_IMAGE_TILING_OPTIMAL;
	image_desc.samples			= VK_SAMPLE_COUNT_1_BIT;
	image_desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;//The resource state undefine at the begining.
	image_desc.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	image_desc.flags			= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	if (vkCreateImage(p_MemorySpace->GetDevice()->device, &image_desc, nullptr, &p_Image) != VK_SUCCESS)
	{
		throw std::runtime_error("CTextureCube :: CreateBuffer Error -> Failed to create the texture buffer!");
		return;
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(p_DeviceContext->device, p_Image, &memory_requirements);
	m_RequiredSize	= memory_requirements.size;
	m_Layout		= image_desc.initialLayout;

	m_BindingLocation = p_MemorySpace->BindTexture(this);

	if(construct_data_ != nullptr && upload_task_ != nullptr)
		Upload(construct_data_, upload_task_);
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
	
	if (vkCreateImageView(p_DeviceContext->device, &view_desc, nullptr, &p_ImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("CTexture2D::CreateView -> Failed to create image view");
		return;
	}
}

Void CTextureCube::Upload(Pointer construct_data_, CGpuUploadTask* upload_task_)
{
	Uint layer_bytes = m_Width * m_Height * vk_format_table.at(m_Format).size;
	CIntermidiateBuffer* upload_buffer = new CIntermidiateBuffer(p_DeviceContext, layer_bytes * 6, CIntermidiateBuffer::Usage::Upload);
	
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