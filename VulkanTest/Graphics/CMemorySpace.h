#pragma once
#include "CAllocator.h"

#define	ALLOCATION_FAIELD UINT32_MAX

enum MemoryRigionSize
{
	__16MB  = (1 << 24),
	__32MB  = (1 << 25),
	__64MB  = (1 << 26),
	__128MB = (1 << 27),
	__256MB = (1 << 28),
	__512MB = (1 << 29)
};


class CVulkanDevice;

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
			Pointer					mapped_memory;

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

		const Uint				m_MemoryTypeIndex;
		const MemoryRigionSize	m_RigionSize;

		Bool m_IsMapped;

		CVulkanDevice* const p_Device;

	public:
		CMemorySpace(CVulkanDevice* device_, VkMemoryPropertyFlags memory_properties_, MemoryRigionSize rigion_size_);

		//The function return the binding offset and the memory
		BindingLocation BindBuffer(CBoundedBuffer* buffer_);
		BindingLocation BindTexture(CTextureBase* texture_);

		Void Map();
		Void Unmap();

		CVulkanDevice* GetDevice() { return p_Device; }
};
