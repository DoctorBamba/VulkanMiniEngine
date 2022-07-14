#include "CTexture2D.h"

Bool CTextureBase::SupportMemoryType(Uint memory_type_)
{
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(p_DeviceContext->device, p_Image, &memory_requirements);

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

	vkUpdateDescriptorSets(p_DeviceContext->device, 1, &descriptor_write, 0, nullptr);
}

CTextureBase::~CTextureBase()
{
	if (p_ImageView != nullptr)
		vkDestroyImageView(p_DeviceContext->device, p_ImageView, nullptr);

	if (p_Image != nullptr)
		vkDestroyImage(p_DeviceContext->device, p_Image, nullptr);
}