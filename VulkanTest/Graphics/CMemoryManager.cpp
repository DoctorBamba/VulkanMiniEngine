#include "CDevice.h"

#include "Buffers/CBuffer.h"
#include "CTexture2D.h"


CMemorySpace::CMemorySpace(CVulkanDevice* device_, VkMemoryPropertyFlags memory_properties_, MemoryRigionSize rigion_size_)
			: p_Device(device_)
			, m_MemoryTypeIndex(device_->FindSupportedMemoryType(0xffffffff, memory_properties_))
			, m_RigionSize(rigion_size_)
{
	m_IsMapped = false;
	p_FirstContinuity = new ContinuityRigion(device_->device, m_RigionSize, m_MemoryTypeIndex, nullptr);
}

Void CMemorySpace::Map()
{
	if (m_IsMapped)
	{
		throw std::runtime_error("CMemorySpace :: Map -> This memory space is allready mapped!");
		return;
	}

	ContinuityRigion* rigion = p_FirstContinuity;
	while (rigion != nullptr)
	{
		vkMapMemory(p_Device->device, rigion->memory, 0, rigion->GetSize(), 0, &rigion->mapped_memory);
		rigion = rigion->next;
	}

	m_IsMapped = true;
}

Void CMemorySpace::Unmap()
{
	if (!m_IsMapped)
	{
		throw std::runtime_error("CMemorySpace :: Unmap -> Try to unmaping an unmaped memory!");
		return;
	}

	ContinuityRigion* rigion = p_FirstContinuity;
	while (rigion != nullptr)
	{
		vkUnmapMemory(p_Device->device, rigion->memory);
		rigion->mapped_memory = nullptr;
		rigion = rigion->next;
	}

	m_IsMapped = false;
}

CMemorySpace::BindingLocation CMemorySpace::BindBuffer(CBoundedBuffer* buffer_)
{
	if (!buffer_->SupportMemoryType(m_MemoryTypeIndex))
	{
		throw std::runtime_error("CMemorySpace::BindBuffer Error -> The given buffer not support this memory type!");
		return CMemorySpace::BindingLocation();
	}

	//Find Continuity-Rigion that have enough space for this buffer...
	ContinuityRigion* rigion = p_FirstContinuity;
	while (!rigion->AllocatePossible(buffer_->GetRequiredSize()))
	{
		if (rigion->next == nullptr)//If rigion not found create a new empty one...
		{
			rigion->next = new ContinuityRigion(p_Device->device, m_RigionSize, m_MemoryTypeIndex, rigion);
			if (m_IsMapped)
				vkMapMemory(p_Device->device, rigion->next->memory, 0, rigion->next->GetSize(), 0, &rigion->next->mapped_memory);
		}
		rigion = rigion->next;
	}

	//Bind buffer on the rigion...
	Uint memory_offset = rigion->Allocate(buffer_->GetRequiredSize());
	vkBindBufferMemory(p_Device->device, buffer_->GetVkBuffer(), rigion->memory, memory_offset);

	CMemorySpace::BindingLocation binding_location;
	binding_location.rigion = rigion;
	binding_location.offset = memory_offset;

	return binding_location;
}

CMemorySpace::BindingLocation CMemorySpace::BindTexture(CTextureBase* texture_)
{
	if (!texture_->SupportMemoryType(m_MemoryTypeIndex))
	{
		throw std::runtime_error("CMemorySpace::BindTexture Error -> The given texture not support this memory type!");
		return CMemorySpace::BindingLocation();
	}

	//Find Continuity-Rigion that have enough space for this buffer...
	ContinuityRigion* rigion = p_FirstContinuity;
	while (!rigion->AllocatePossible(texture_->GetRequiredSize()))
	{
		if (rigion->next == nullptr)//If rigion not found create a new empty one...
		{
			rigion->next = new ContinuityRigion(p_Device->device, m_RigionSize, m_MemoryTypeIndex, rigion);
			if (m_IsMapped)
				vkMapMemory(p_Device->device, rigion->next->memory, 0, rigion->next->GetSize(), 0, &rigion->next->mapped_memory);
		}
		rigion = rigion->next;
	}

	//Bind texture on the rigion...
	Uint memory_offset = rigion->Allocate(texture_->GetRequiredSize());
	vkBindImageMemory(p_Device->device, texture_->GetVkImage(), rigion->memory, memory_offset);

	CMemorySpace::BindingLocation binding_location;
	binding_location.rigion = rigion;
	binding_location.offset = memory_offset;

	return binding_location;
}