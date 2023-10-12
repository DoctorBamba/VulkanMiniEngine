#include "CTexture2D.h"

Bool CTextureBase::SupportMemoryType(Uint memory_type_)
{
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(p_Device->device, p_Image, &memory_requirements);

	return (Bool)(memory_requirements.memoryTypeBits & memory_type_);
}

VkAccessFlagBits CTextureBase::GetRequierdAccessFlag(VkImageLayout image_layout_)
{
	static std::map<VkImageLayout, VkAccessFlagBits> access_map =
	{
		{VK_IMAGE_LAYOUT_UNDEFINED, VK_ACCESS_NONE_KHR},
		{VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT},
		{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
		{VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_NONE_KHR},
		{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_NONE_KHR}
	};

	auto find = access_map.find(image_layout_);
	if (find != access_map.end())
		return find->second;
	else
		return VK_ACCESS_NONE_KHR;
}

VkImageSubresourceRange CTextureBase::GetSubresourceRange()
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
	subresource_range.layerCount		= m_Layers;

	return subresource_range;
}

const VkImageType TextureDimensionToImageTypeTable[] =
{
	VK_IMAGE_TYPE_1D,
	VK_IMAGE_TYPE_2D,
	VK_IMAGE_TYPE_3D,
	VK_IMAGE_TYPE_2D,
	VK_IMAGE_TYPE_2D,
	VK_IMAGE_TYPE_2D
};

const VkImageCreateFlags TextureDimensionToCompatibilityFlagTable[] =
{
	0,
	0,
	0,
	VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
	VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
	VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT | VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
};

Void CTextureBase::CreateBuffer(Bool attachment_usage_, Pointer construct_data_, CGpuUploadTask* upload_task_)
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

	image_desc.imageType		= TextureDimensionToImageTypeTable[(Uint)m_Dimension];
	image_desc.format			= m_Format;
	image_desc.extent.width		= m_Width;
	image_desc.extent.height	= m_Height;
	image_desc.extent.depth		= 1;
	image_desc.arrayLayers		= m_Layers;
	image_desc.mipLevels		= 1;
	image_desc.usage			= usage;
	image_desc.tiling			= VK_IMAGE_TILING_OPTIMAL;
	image_desc.samples			= VK_SAMPLE_COUNT_1_BIT;
	image_desc.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;//The resource state undefine at the begining.
	image_desc.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	image_desc.flags			= TextureDimensionToCompatibilityFlagTable[(Uint)m_Dimension];

	if (vkCreateImage(p_MemorySpace->GetDevice()->device, &image_desc, nullptr, &p_Image) != VK_SUCCESS)
	{
		throw std::runtime_error("CTexture2D :: CreateBuffer Error -> Failed to create the texture buffer!");
		return;
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(p_Device->device, p_Image, &memory_requirements);
	m_RequiredSize	= memory_requirements.size;
	m_Layout		= image_desc.initialLayout;

	m_BindingLocation = p_MemorySpace->BindTexture(this);

	if(construct_data_ != nullptr && upload_task_ != nullptr)
		Upload(construct_data_, upload_task_);
}


VkImageMemoryBarrier CTextureBase::BarrierTranslation(VkImageLayout new_layout_)
{
	VkImageMemoryBarrier barrier;
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext							= nullptr;
	
	barrier.srcAccessMask					= GetRequierdAccessFlag(m_Layout);
	barrier.dstAccessMask					= GetRequierdAccessFlag(new_layout_);
	barrier.oldLayout						= m_Layout;
	barrier.newLayout						= new_layout_;
	barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= p_Image;
	barrier.subresourceRange				= GetSubresourceRange();

	m_Layout = new_layout_;

	return barrier;
}

Void CTextureBase::Barrier(VkImageLayout new_layout_, CGpuTask* task_)
{
	VkImageMemoryBarrier barrier = BarrierTranslation(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkCmdPipelineBarrier(task_->GetCommandBuffer(), 0, 0, 0, 0, 0, 0, 0, 1, &barrier);
}

Void CTextureBase::Bind(VkDescriptorSet descriptor_set_, Uint binding_, Uint array_index_) const
{
	VkDescriptorImageInfo image_info{};
	image_info.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView	= p_ImageView;
	image_info.sampler		= nullptr;

	VkWriteDescriptorSet descriptor_write{};
	descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet				= descriptor_set_;
	descriptor_write.dstBinding			= binding_;
	descriptor_write.dstArrayElement	= array_index_;
	descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptor_write.descriptorCount	= 1;
	descriptor_write.pBufferInfo		= nullptr;
	descriptor_write.pImageInfo			= &image_info;
	descriptor_write.pTexelBufferView	= nullptr;

	vkUpdateDescriptorSets(p_Device->device, 1, &descriptor_write, 0, nullptr);
}

CTextureBase::~CTextureBase()
{
	if (p_ImageView != nullptr)
		vkDestroyImageView(p_Device->device, p_ImageView, nullptr);

	if (p_Image != nullptr)
		vkDestroyImage(p_Device->device, p_Image, nullptr);
}