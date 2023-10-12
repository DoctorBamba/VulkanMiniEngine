#pragma once
#include "../CDevice.h"

class CBufferBase
{
	protected:
		VkBuffer				p_Buffer;
		
		Uint					m_Size;
		Uint					m_RequiredSize;

		CVulkanDevice* const p_Device;

	protected:
		CBufferBase(CVulkanDevice* device_) : p_Device(device_) {}

	public:
		Bool SupportMemoryType(Uint memory_type_)
		{
			VkMemoryRequirements memory_requirements;
			vkGetBufferMemoryRequirements(p_Device->device, p_Buffer, &memory_requirements);

			return (Bool)(memory_requirements.memoryTypeBits & memory_type_);
		}

		virtual ~CBufferBase()
		{
			if (p_Buffer != VK_NULL_HANDLE)
				vkDestroyBuffer(p_Device->device, p_Buffer, nullptr);
		}

		Uint GetSize() { return m_Size; }
		Uint GetRequiredSize() { return m_RequiredSize; }
		VkBuffer GetVkBuffer() { return p_Buffer; }
};

//Independant Buffer is buffer With its own device memory
class CIndependentBuffer : public CBufferBase
{
	protected:
		VkDeviceMemory p_DeviceMemory;

	public:
		CIndependentBuffer(CVulkanDevice* device_,  Uint allocate_size_, Uint usage_flag_,
							VkMemoryPropertyFlags memory_requirments_ = 0, Bool shared_ = false)
								: CBufferBase(device_)
		{
			m_Size = allocate_size_;

			//Create the buffer...
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size			= allocate_size_;
			bufferInfo.usage		= usage_flag_;
			bufferInfo.sharingMode	= shared_ ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			
			if (vkCreateBuffer(p_Device->device, &bufferInfo, nullptr, &p_Buffer) != VK_SUCCESS)
			{
				throw std::runtime_error("CBuffer::CBuffer -> failed to create buffer!");
				return;
			}

			//Allocate memory...
			VkMemoryRequirements memory_requirements;
			vkGetBufferMemoryRequirements(p_Device->device, p_Buffer, &memory_requirements);

			m_RequiredSize = memory_requirements.size;

			VkMemoryAllocateInfo allocation_desc{};
			allocation_desc.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocation_desc.allocationSize	= memory_requirements.size;
			allocation_desc.memoryTypeIndex	= p_Device->FindSupportedMemoryType(memory_requirements.memoryTypeBits, memory_requirments_);
			
			if (vkAllocateMemory(p_Device->device, &allocation_desc, nullptr, &p_DeviceMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("CBuffer::CBuffer -> Buffer Allocation failed!");
				return;
			}

			//Bind buffer memory...
			if (vkBindBufferMemory(p_Device->device, p_Buffer, p_DeviceMemory, 0) != VK_SUCCESS)
			{
				throw std::runtime_error("CBuffer::CBuffer -> Failed to bind buffer memory!");
				return;
			}
		}

		virtual ~CIndependentBuffer()
		{
			if (p_DeviceMemory)
				vkFreeMemory(p_Device->device, p_DeviceMemory, nullptr);
		}

};

//Bounded Buffer is a buffer that bounded inside some external device memory
class CBoundedBuffer : public CBufferBase
{
	protected:
		CMemorySpace::BindingLocation m_BindingLocation;
		CMemorySpace* const m_MemorySpace;

	public:
		CBoundedBuffer(CMemorySpace* memory_space_, Uint allocate_size_, Uint usage_flag_, Bool shared_ = false)
			: CBufferBase(memory_space_->GetDevice()), m_MemorySpace(memory_space_)
		{
			m_Size = allocate_size_;

			//Create the buffer...
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size			= allocate_size_;
			bufferInfo.usage		= usage_flag_;
			bufferInfo.sharingMode	= shared_ ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
			
			if (vkCreateBuffer(p_Device->device, &bufferInfo, nullptr, &p_Buffer) != VK_SUCCESS)
			{
				throw std::runtime_error("CBuffer::CBuffer -> failed to create buffer!");
				return;
			}

			VkMemoryRequirements memory_requirements;
			vkGetBufferMemoryRequirements(p_Device->device, p_Buffer, &memory_requirements);

			m_RequiredSize = memory_requirements.size;
			
			m_BindingLocation = memory_space_->BindBuffer(this);
		}

		Byte* Get_MappedMemory()
		{
			if(m_BindingLocation.rigion->mapped_memory == nullptr)
			{
				throw std::runtime_error("CBoundedBuffer :: GetMappedMemory Error -> The memory that this buffer bounded on is not mapped!");
				return nullptr;
			}

			return (reinterpret_cast<Byte*>(m_BindingLocation.rigion->mapped_memory) + m_BindingLocation.offset);
		}
};