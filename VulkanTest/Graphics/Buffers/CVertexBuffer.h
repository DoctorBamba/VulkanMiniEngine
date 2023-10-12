#pragma once
#include "CLocalBuffer.h"

template<class VertexType>
class CVertexBuffer : public CLocalBuffer
{
	public:
		CVertexBuffer(CVulkanDevice* device_, Uint vertices_count_,
					   CGpuUploadTask* upload_task_ = nullptr, Pointer initional_data_ = nullptr)
						: CLocalBuffer(device_, sizeof(VertexType) * vertices_count_, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, upload_task_, initional_data_){}

		Void Bind(VkCommandBuffer command_buffer_, Uint location_, VkDeviceSize stride_)
		{
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers2EXT(command_buffer_, location_, 1, &p_Buffer, &offset, nullptr, &stride_);
		}
};


typedef unsigned int IndexType;
typedef IndexType* IndicesStream;

class CIndexBuffer : public CLocalBuffer
{
	public:
		CIndexBuffer(CVulkanDevice* device_, Uint indices_count_,
						CGpuUploadTask* upload_task_ = nullptr, IndicesStream initional_data_ = nullptr)
							: CLocalBuffer(device_, sizeof(IndexType) * indices_count_, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, upload_task_, initional_data_){}

		Void Bind(VkCommandBuffer command_buffer_)
		{
			vkCmdBindIndexBuffer(command_buffer_, p_Buffer, 0, VK_INDEX_TYPE_UINT32);
		}
};