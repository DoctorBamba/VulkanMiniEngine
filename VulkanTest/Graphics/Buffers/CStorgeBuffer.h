#pragma once
#include "CLocalBuffer.h"

static Uint ComputeDynamicStorgeAligment(VulkanDevice* device_, Uint size_)
{
	Uint min_sbo_aligment = device_->properties.limits.minStorageBufferOffsetAlignment;
	return (min_sbo_aligment > 0) ? (size_ + min_sbo_aligment - 1) & ~(min_sbo_aligment - 1) : size_;
}

template<class T>
class CStorgeBuffer : public CLocalBuffer
{
	private:
		const Uint m_AligmentSize;
		const Uint m_ElementsCount;

	public:
		CStorgeBuffer(VulkanDevice* device_, Uint elements_count_, Pointer initional_data_ = nullptr, CGpuUploadTask* upload_task_ = nullptr)
						: CLocalBuffer(device_, ComputeDynamicStorgeAligment(device_, sizeof(T)) * elements_count_, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, upload_task_, initional_data_), m_AligmentSize(m_Size / elements_count_), m_ElementsCount(elements_count_){}

		Void Bind(VkDescriptorSet descriptor_set_, Uint binding_)
		{
			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = p_Buffer;
			buffer_info.offset = 0;
			buffer_info.range  = m_AligmentSize * m_ElementsCount;

			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet				= descriptor_set_;
			descriptor_write.dstBinding			= binding_;
			descriptor_write.dstArrayElement	= 0;
			descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptor_write.descriptorCount	= 1;
			descriptor_write.pBufferInfo		= &buffer_info;
			descriptor_write.pImageInfo			= nullptr;
			descriptor_write.pTexelBufferView	= nullptr;

			vkUpdateDescriptorSets(p_DeviceContext->device, 1, &descriptor_write, 0, nullptr);	
		}

		Uint GetElementAligmentize() { return m_AligmentSize; }
};