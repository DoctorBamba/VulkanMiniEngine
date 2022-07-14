#include "Buffers/CBuffer.h"
#include "CTexture2D.h"

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
			rigion->next = new ContinuityRigion(p_DeviceContext->device, m_RigionSize, m_MemoryTypeIndex, rigion);
			if (m_IsMapped)
				vkMapMemory(p_DeviceContext->device, rigion->next->memory, 0, rigion->next->GetSize(), 0, &rigion->next->mapped_memory);
		}
		rigion = rigion->next;
	}

	//Bind buffer on the rigion...
	Uint memory_offset = rigion->Allocate(buffer_->GetRequiredSize());
	vkBindBufferMemory(p_DeviceContext->device, buffer_->GetVkBuffer(), rigion->memory, memory_offset);

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
			rigion->next = new ContinuityRigion(p_DeviceContext->device, m_RigionSize, m_MemoryTypeIndex, rigion);
			if (m_IsMapped)
				vkMapMemory(p_DeviceContext->device, rigion->next->memory, 0, rigion->next->GetSize(), 0, &rigion->next->mapped_memory);
		}
		rigion = rigion->next;
	}

	//Bind texture on the rigion...
	Uint memory_offset = rigion->Allocate(texture_->GetRequiredSize());
	vkBindImageMemory(p_DeviceContext->device, texture_->GetVkImage(), rigion->memory, memory_offset);

	CMemorySpace::BindingLocation binding_location;
	binding_location.rigion = rigion;
	binding_location.offset = memory_offset;

	return binding_location;
}