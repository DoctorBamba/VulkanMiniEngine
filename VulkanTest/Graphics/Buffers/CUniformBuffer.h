#pragma once
#include "CBuffer.h"
#include "../CMemoryManager.h"

template<class T>
class CUniformBuffer : public CBoundedBuffer
{
	static const Uint structure_size = sizeof(T);

	public:
		CUniformBuffer(CMemorySpace* uniform_space_)
			: CBoundedBuffer(uniform_space_, structure_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT){}

		Void Update(const T& value_)
		{
			memcpy(Get_MappedMemory(), &value_, structure_size);
		}

		Void Bind(VkDescriptorSet descriptor_set_, Uint binding_)
		{
			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = p_Buffer;
			buffer_info.offset = 0;
			buffer_info.range  = structure_size;

			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet				= descriptor_set_;
			descriptor_write.dstBinding			= binding_;
			descriptor_write.dstArrayElement	= 0;
			descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_write.descriptorCount	= 1;
			descriptor_write.pBufferInfo		= &buffer_info;
			descriptor_write.pImageInfo			= nullptr;
			descriptor_write.pTexelBufferView	= nullptr;

			vkUpdateDescriptorSets(p_DeviceContext->device, 1, &descriptor_write, 0, nullptr);
		}
};

static Uint ComputeDynamicUniformAligment(VulkanDevice* device_, Uint size_)
{
	Uint min_ubo_aligment = device_->properties.limits.minUniformBufferOffsetAlignment;
	return (min_ubo_aligment > 0) ? (size_ + min_ubo_aligment - 1) & ~(min_ubo_aligment - 1) : size_;
}

template<class T, Uint n>
class CDynamicUniformBuffer : public CBoundedBuffer
{
	static const Uint structure_size = sizeof(T);

	private:
		const Uint aligment_size;

	public:
		CDynamicUniformBuffer(CMemorySpace* uniform_space_) : CBoundedBuffer(uniform_space_, ComputeDynamicUniformAligment(uniform_space_->GetDevice(), structure_size) * n, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT), aligment_size(m_Size / n) {}

		Void Update(const T& value, Uint index_)
		{
			T* block_pointer = reinterpret_cast<T*>(Get_MappedMemory() + (Long)(aligment_size * index_));
			*block_pointer = value;
			int y = 5;
		}

		Void Bind(VkDescriptorSet descriptor_set_, Uint binding_, Uint visible_counts_ = 1u)
		{
			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = p_Buffer;
			buffer_info.offset = 0;
			buffer_info.range  = aligment_size * visible_counts_;

			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet				= descriptor_set_;
			descriptor_write.dstBinding			= binding_;
			descriptor_write.dstArrayElement	= 0;
			descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptor_write.descriptorCount	= 1;
			descriptor_write.pBufferInfo		= &buffer_info;
			descriptor_write.pImageInfo			= nullptr;
			descriptor_write.pTexelBufferView	= nullptr;

			vkUpdateDescriptorSets(p_DeviceContext->device, 1, &descriptor_write, 0, nullptr);
		}

		Uint GetAligmentSize() { return aligment_size; }
};