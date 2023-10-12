#include "CTexture2D.h"

std::vector<CTexture2D*> TexturesFromBackBuffers(CVulkanDevice* device_, VkSwapchainKHR swap_chain_, Uint16 buffers_width_, Uint16 buffer_height_, VkFormat buffers_format_)
{
	std::vector<CTexture2D*> textures;

	Uint back_buffers_count;
	vkGetSwapchainImagesKHR(device_->device, swap_chain_, &back_buffers_count, nullptr);

	std::vector<VkImage> back_buffers(back_buffers_count);
	vkGetSwapchainImagesKHR(device_->device, swap_chain_, &back_buffers_count, back_buffers.data());

	for(Uint i = 0 ; i < back_buffers_count ; i ++)
	{
		CTexture2D* texture = new CTexture2D(device_);
		texture->m_Name	  = std::string("BackBufferTexture_") + std::to_string(i);
		texture->m_Width  = buffers_width_;
		texture->m_Height = buffer_height_;
		texture->m_Layers = 1;
		texture->m_Format = buffers_format_;
		texture->p_Image  = back_buffers[i];

		texture->CreateView();

		textures.push_back(texture);
	}

	return textures;
}

CTexture2D::CTexture2D(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, VkFormat format_, Bool attachment_usage_,
						Pointer construct_data_, CGpuUploadTask* upload_task_, std::string name_) : CTextureBase(memory_space_, Dimension::Texture2D, CreationWay::CREATE_FROM_DATA)
{
	m_Name		= name_;
	m_Width		= width_;
	m_Height	= height_;
	m_Format	= format_;
	m_Layers	= 1;
	
	CreateBuffer(attachment_usage_, construct_data_, upload_task_);
	CreateView();
}

CTexture2D::CTexture2D(CMemorySpace* memory_space_, std::wstring image_directory_, CGpuUploadTask* upload_task_) : CTextureBase(memory_space_, Dimension::Texture2D, CreationWay::CREATE_FROM_FILE)
{
	if (upload_task_ == nullptr)
	{
		throw std::runtime_error("CTexture2D :: Load Image Error -> Cant Upload image data to Gpu becouse the given Uplaod Task is nullptr!");
		return;
	}

	//Load image

	if (!DirectoryExist(image_directory_.c_str()))
		return;

	FREE_IMAGE_FORMAT file_format;
	file_format = FreeImage_GetFileTypeU(image_directory_.c_str(), 0);
	if (file_format == FIF_UNKNOWN)
	{
		std::string str_dir = std::string(image_directory_.begin(), image_directory_.end());
		throw std::runtime_error("CTexture2D :: Load Image Error -> The texture loader cant load this file format or the file not exist(Directory : " + str_dir + ")");
		return;
	}

	m_ImagePath = image_directory_;

	FIBITMAP* h_image = FreeImage_LoadU(file_format, image_directory_.c_str());
	if (h_image == nullptr)
	{
		throw std::runtime_error("CTexture2D :: Load Image Error -> The texture loader failed to load the image");
		return;
	}

	FIBITMAP* temp = h_image;
	h_image = FreeImage_ConvertTo32Bits(h_image);

	//Set texture parameters
	m_Width  = FreeImage_GetWidth(h_image);
	m_Height = FreeImage_GetHeight(h_image);
	m_Layers = 1;

	//Reorder image data...
	
	Uint  pixel_bytes	= FreeImage_GetPitch(h_image) / m_Width;
	Byte* temp_data		= FreeImage_GetBits(h_image);
	Byte* data_pointer = new BYTE[(Uint)m_Width * (Uint)m_Height * 4];

	Uint dest_loc, src_loc;
	for (uint32_t i = 0; i < m_Width; i++)
	{
		for (uint32_t j = 0; j < m_Height; j++)
		{
			src_loc  = (j * m_Width + i) * 4;
			dest_loc = ((m_Height - j - 1) * m_Width + i) * 4;
			data_pointer[dest_loc + 0] = temp_data[src_loc + 2];
			data_pointer[dest_loc + 1] = temp_data[src_loc + 1];
			data_pointer[dest_loc + 2] = temp_data[src_loc + 0];
			data_pointer[dest_loc + 3] = temp_data[src_loc + 3];
		}
	}

	FreeImage_Unload(temp);

	//Define the format...
	switch (pixel_bytes)
	{
		case 1U:
			m_Format = VK_FORMAT_R8_UNORM;
			break;
		case 2U:
			m_Format = VK_FORMAT_R8G8_UNORM;
			break;
		case 4U:
			m_Format = VK_FORMAT_R8G8B8A8_UNORM;
			break;

		default:
			throw std::runtime_error("CTexture2D :: Load Image Error -> The Image's pixel size is not supported");
			break;
	}

	CreateBuffer(false, data_pointer, upload_task_);
	CreateView();
}

Void CTexture2D::CreateView()
{
	VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

	if (IsDepthFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
	else if (IsColorFormat(m_Format))
		aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;


	VkImageViewCreateInfo view_desc{};
	view_desc.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_desc.image								= p_Image;
	view_desc.viewType							= VK_IMAGE_VIEW_TYPE_2D;
	view_desc.format							= m_Format;
	view_desc.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
	view_desc.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
	view_desc.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
	view_desc.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
	view_desc.subresourceRange					= GetSubresourceRange();
	
	if (vkCreateImageView(p_Device->device, &view_desc, nullptr, &p_ImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("CTexture2D::CreateView -> Failed to create image view");
		return;
	}
}

Void CTexture2D::Upload(Pointer construct_data_, CGpuUploadTask* upload_task_)
{
	Uint size = m_Width * m_Height * vk_format_table.at(m_Format).size;
	CIntermidiateBuffer* upload_buffer = new CIntermidiateBuffer(p_Device, size, CIntermidiateBuffer::Usage::Upload);
	memcpy(upload_buffer->Map(), construct_data_, size);
	upload_buffer->Unmap();

	VkBufferImageCopy region{};
	region.bufferOffset						= 0;
	region.bufferRowLength					= 0;
	region.bufferImageHeight				= 0;
	region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.layerCount		= 1;
	region.imageSubresource.baseArrayLayer	= 0;
	region.imageSubresource.mipLevel		= 0;
	region.imageOffset						= {0, 0, 0};
	region.imageExtent						= {m_Width, m_Height, 1};

	VkImageMemoryBarrier begin_barrier = BarrierTranslation(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdPipelineBarrier(upload_task_->GetCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &begin_barrier);
	
	vkCmdCopyBufferToImage(upload_task_->GetCommandBuffer(), upload_buffer->GetVkBuffer(), p_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	
	VkImageMemoryBarrier after_barrier = BarrierTranslation(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkCmdPipelineBarrier(upload_task_->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, 0, 0, 0, 1, &after_barrier);

	upload_task_->m_Intermidiates.push_back(upload_buffer);
}