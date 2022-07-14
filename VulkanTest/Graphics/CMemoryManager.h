#pragma once
#include "CAllocator.h"

#define	ALLOCATION_FAIELD UINT32_MAX

Uint FindSupportedMemoryType(VkPhysicalDevice physical_device_, Uint memory_type_flag_, Uint addition_type_requirments_);

enum MemoryRigionSize
{
	__16MB  = (1 << 24),
	__32MB  = (1 << 25),
	__64MB  = (1 << 26),
	__128MB = (1 << 27),
	__256MB = (1 << 28),
	__512MB = (1 << 29)
};


class CBoundedBuffer;
class CTextureBase;

class CMemorySpace
{
	friend CBoundedBuffer;
	friend CTextureBase;

	class ContinuityRigion : public CBlocksAllocator
	{
		public:
			VkDeviceMemory			memory;
			Pointer				mapped_memory;

			const ContinuityRigion* previous;
			ContinuityRigion*		next;

		public:
			ContinuityRigion(VkDevice device_, MemoryRigionSize size_, Uint memory_type_index, ContinuityRigion* previous_)
				: CBlocksAllocator(size_), mapped_memory(nullptr), previous(previous_), next(nullptr)
			{

				VkMemoryAllocateInfo allocation_desc{};
				allocation_desc.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocation_desc.allocationSize	= size_;
				allocation_desc.memoryTypeIndex = memory_type_index;

				if (vkAllocateMemory(device_, &allocation_desc, nullptr, &memory) != VK_SUCCESS)
				{
					throw std::runtime_error("Graphics Memory Allocation failed!");
					return;
				}
			}
	};

	struct BindingLocation
	{
		ContinuityRigion*	rigion;
		Uint				offset;
	};

	private:
		ContinuityRigion* p_FirstContinuity;

		const Uint			m_MemoryTypeIndex;
		const MemoryRigionSize	m_RigionSize;

		Bool m_IsMapped;

		VulkanDevice* const p_DeviceContext;

	public:
		CMemorySpace(VulkanDevice* device_, VkMemoryPropertyFlags memory_properties_, MemoryRigionSize rigion_size_)
			: p_DeviceContext(device_)
			, m_MemoryTypeIndex(FindSupportedMemoryType(device_->physical_device, 0xffffffff, memory_properties_))
			, m_RigionSize(rigion_size_)
		{
			m_IsMapped = false;
			p_FirstContinuity = new ContinuityRigion(device_->device, m_RigionSize, m_MemoryTypeIndex, nullptr);
		}

		//The function return the binding offset and the memory
		BindingLocation BindBuffer(CBoundedBuffer* buffer_);
		BindingLocation BindTexture(CTextureBase* texture_);


		Void Map()
		{
			if (m_IsMapped)
			{
				throw std::runtime_error("CMemorySpace :: Map -> This memory space is allready mapped!");
				return;
			}

			ContinuityRigion* rigion = p_FirstContinuity;
			while (rigion != nullptr)
			{
				vkMapMemory(p_DeviceContext->device, rigion->memory, 0, rigion->GetSize(), 0, &rigion->mapped_memory);
				rigion = rigion->next;
			}

			m_IsMapped = true;
		}

		Void Unmap()
		{
			if (!m_IsMapped)
			{
				throw std::runtime_error("CMemorySpace :: Unmap -> Try to unmaping an unmaped memory!");
				return;
			}

			ContinuityRigion* rigion = p_FirstContinuity;
			while (rigion != nullptr)
			{
				vkUnmapMemory(p_DeviceContext->device, rigion->memory);
				rigion->mapped_memory = nullptr;
				rigion = rigion->next;
			}

			m_IsMapped = false;
		}

		VulkanDevice* GetDevice() { return p_DeviceContext; }
};
