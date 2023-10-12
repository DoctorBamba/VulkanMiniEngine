#pragma once
#include "CBuffer.h"
#include "../CGpuTask.h"


class CIntermidiateBuffer : public CIndependentBuffer
{
	public:
		enum class Usage
		{
			Upload,
			Download
		};

	private:
		const Usage m_Usage;
		Pointer m_MappeData;

	public:
		CIntermidiateBuffer(CVulkanDevice* device_, Uint allocate_size_, Usage usage_)
			: CIndependentBuffer(device_, 
					  allocate_size_, 
					  (usage_ == Usage::Upload) ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
					  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
					  false), m_Usage(usage_) {}

		virtual ~CIntermidiateBuffer()
		{
			Unmap();
		}

		Pointer Map()
		{
			if (m_MappeData != nullptr)
			{
				throw std::runtime_error("CIntermidiateBuffer :: Map -> The Device Memory of this buffer is allready mapped!");
				return m_MappeData;
			}
			
			vkMapMemory(p_Device->device, p_DeviceMemory, 0, m_Size, 0, &m_MappeData);

			return m_MappeData;
		}

		Void Unmap()
		{
			if (m_MappeData != nullptr)
			{
				vkUnmapMemory(p_Device->device, p_DeviceMemory);
				m_MappeData = nullptr;
			}
		}

		Usage GetUsage() { return m_Usage; }

};