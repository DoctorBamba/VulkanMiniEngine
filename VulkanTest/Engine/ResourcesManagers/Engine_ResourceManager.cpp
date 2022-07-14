#include "Engine_ResourcesManager.h"

Void Engine::ResourcesMenager::CreateStaticDescriptorSet(Uint textures_count_limit_)
{
	//Create descriptor pool...
    VkDescriptorPoolSize materials_size{};
	materials_size.type             = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	materials_size.descriptorCount  = 1;

    VkDescriptorPoolSize textures_size{};
	textures_size.type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textures_size.descriptorCount	= textures_count_limit_;

    VkDescriptorPoolSize pool_sizes[] = { materials_size, textures_size };

    VkDescriptorPoolCreateInfo pool_desc{};
    pool_desc.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_desc.poolSizeCount = array_size(pool_sizes);
    pool_desc.pPoolSizes    = pool_sizes;
    pool_desc.maxSets       = 1;

    if (vkCreateDescriptorPool(p_DeviceContext->device, &pool_desc, nullptr, &static_descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("ResourcesMenager :: CreateStaticDescriptorSet -> Failed to create the descriptor pool!");
        return;
    }

	//Create the static descriptor set layout...

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_desc{};

	VkDescriptorSetLayoutBinding materials_binding{};
	materials_binding.binding				= 0;
	materials_binding.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	materials_binding.descriptorCount		= 1;
	materials_binding.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	VkDescriptorSetLayoutBinding textures_binding{};
	textures_binding.binding				= 1;
	textures_binding.descriptorType			= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textures_binding.descriptorCount		= textures_count_limit_;
	textures_binding.stageFlags				= VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	VkDescriptorSetLayoutBinding statics_binds[] = { materials_binding, textures_binding };

	VkDescriptorBindingFlags flags[2];
	flags[0] = 0;
	flags[1] = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

	VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags{};
	binding_flags.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	binding_flags.bindingCount	= 2;
	binding_flags.pBindingFlags = flags;

	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(statics_binds);
	descriptor_set_layout_desc.pBindings	= statics_binds;
	descriptor_set_layout_desc.pNext		= &binding_flags;

	if (vkCreateDescriptorSetLayout(p_DeviceContext->device, &descriptor_set_layout_desc, nullptr, &static_descriptor_set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("ResourcesMenager :: CreateStaticDescriptorSet -> Failed to create descriptor set layout!");
		return;
	}

	VkDescriptorSetVariableDescriptorCountAllocateInfo allocation_info_set_counts = {};
	allocation_info_set_counts.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
	allocation_info_set_counts.descriptorSetCount	= 1;
	allocation_info_set_counts.pDescriptorCounts	= &textures_count_limit_;

	//Alocate the static descriptor set...
	VkDescriptorSetAllocateInfo allocation_info{};
	allocation_info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocation_info.descriptorPool		= static_descriptor_pool;
	allocation_info.descriptorSetCount	= 1;
	allocation_info.pSetLayouts			= &static_descriptor_set_layout;
	allocation_info.pNext				= &allocation_info_set_counts;

	if (vkAllocateDescriptorSets(p_DeviceContext->device, &allocation_info, &static_descriptor_set) != VK_SUCCESS)
	{
		throw std::runtime_error("ResourcesMenager :: CreateStaticDescriptorSet Error -> Failed to allocate descriptor sets!");
		return;
	}

	materials_buffer->Bind(static_descriptor_set, 0);

	/*
	//Set unused descriptors...

	CGpuUploadTask* upload_task = new CGpuUploadTask(Engine::graphics->p_Device, Engine::graphics->p_CommandPool);
	upload_task->Reset();
	CTexture2D* unknow_texture_ = new CTexture2D(graphics->m_DeviceMemory.local_space, L"Engine/UnknowTexture.png", upload_task);
	upload_task->Execute(graphics->p_GraphicsQueue);
	upload_task->WaitAntilComplite();
	delete upload_task;

	VkDescriptorImageInfo* images_info = new VkDescriptorImageInfo[textures_count_limit_];

	for (PE_Uint i = 0; i < textures_count_limit_; i++)
	{
		VkDescriptorImageInfo& image_info = images_info[i];
		image_info.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView	= unknow_texture_->GetBufferView();
		image_info.sampler		= Engine::UsefulSamplers::high_quality_linear;
	}

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet				= static_descriptor_set;
	descriptor_write.dstBinding			= 1;
	descriptor_write.dstArrayElement	= 0;
	descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_write.descriptorCount	= textures_count_limit_;
	descriptor_write.pBufferInfo		= nullptr;
	descriptor_write.pImageInfo			= images_info;
	descriptor_write.pTexelBufferView	= nullptr;

	vkUpdateDescriptorSets(p_DeviceContext->device, 1, &descriptor_write, 0, nullptr);
	delete[] images_info;
	*/
}


Engine::ResourcesMenager::ResourcesMenager(VulkanDevice* device_, Uint datablocks_count_limit_, Uint textures_count_limit_) : p_DeviceContext(device_)
{
	materials_buffer		= new CStorgeBuffer<GpuDataBlockStruct>(p_DeviceContext, datablocks_count_limit_, nullptr, nullptr);
	materials_allocator	= new CBlocksAllocator(datablocks_count_limit_);

	textures_allocator		= new CBlocksAllocator(textures_count_limit_);

	CreateStaticDescriptorSet(textures_count_limit_);
}

/***************************Materials********************************/

Void Engine::ResourcesMenager::UploadMaterialsPacket(std::vector<CMaterial*> packet_, CGpuUploadTask* upload_task_)
{
	Uint loc = materials_allocator->Allocate(packet_.size());
				
	CIntermidiateBuffer* upload_buffer = new CIntermidiateBuffer(p_DeviceContext, materials_buffer->GetElementAligmentize() * packet_.size(), CIntermidiateBuffer::Usage::Upload);//Create upload buffer for uploading the material pack
	Byte* data_map = reinterpret_cast<Byte*>(upload_buffer->Map());

	for (Uint i = 0; i < packet_.size() ; i++)
	{
		Byte* entry = data_map + materials_buffer->GetElementAligmentize() * i;
		memcpy(entry, &(packet_.at(i)->Data<GpuDataBlockStruct>()), sizeof(GpuDataBlockStruct));
					
		materials_locations_map.insert(std::pair<CMaterial*, Uint>(packet_.at(i), loc + i));
	}

	upload_buffer->Unmap();
	materials_buffer->Upload(upload_task_, upload_buffer, materials_buffer->GetElementAligmentize() * loc);
}

Uint Engine::ResourcesMenager::GetMaterialLocation(CMaterial* data_block_)
{
	std::map<CMaterial*, Uint>::iterator found = materials_locations_map.find(data_block_);
	if (found != materials_locations_map.end())
		return found->second;

	return INFINITE;
}

/***************************Textures********************************/
Void Engine::ResourcesMenager::AddTexturesPacket(std::vector<CTextureBase*> textures_)
{
	//Rearage the texture array...
	Uint descriptors_counter = 0;
	for (Uint i = 0; i < textures_.size(); i++)
	{
		if (textures_.at(i) == nullptr)
			continue;

		textures_.at(descriptors_counter) = textures_.at(i);
		descriptors_counter ++;
	}

	if (descriptors_counter == 0)
		return;

	textures_.resize(descriptors_counter);

	//Allocate block...
	Uint loc = textures_allocator->Allocate(descriptors_counter);

	VkDescriptorImageInfo* images_info			= new VkDescriptorImageInfo[descriptors_counter];
	VkWriteDescriptorSet*  descriptors_writes	= new VkWriteDescriptorSet[descriptors_counter];
	VkImageMemoryBarrier*  memory_barriers		= new VkImageMemoryBarrier[descriptors_counter];

	for (Uint i = 0; i < descriptors_counter; i++)
	{
		VkDescriptorImageInfo& image_info = images_info[i];
		image_info = {};
		image_info.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView	= textures_.at(i)->GetBufferView();
		image_info.sampler		= Engine::UsefulSamplers::high_quality_linear;
		
		VkWriteDescriptorSet& descriptor_write = descriptors_writes[i];
		descriptor_write = {};
		descriptor_write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet				= static_descriptor_set;
		descriptor_write.dstBinding			= 1;
		descriptor_write.dstArrayElement	= loc + i;
		descriptor_write.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_write.descriptorCount	= 1;
		descriptor_write.pBufferInfo		= nullptr;
		descriptor_write.pImageInfo			= &(images_info[i]);
		descriptor_write.pTexelBufferView	= nullptr;

		memory_barriers[i] = textures_.at(i)->BarrierTranslation(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		textures_locations_map.insert(std::pair<CTextureBase*, Uint>(textures_[i], loc + i));
	}

	vkUpdateDescriptorSets(p_DeviceContext->device, descriptors_counter, descriptors_writes, 0, nullptr);
	
	delete[] images_info;
	delete[] descriptors_writes;
}

Uint Engine::ResourcesMenager::GetTextureLocation(CTextureBase* texture_)
{
	if (texture_ == nullptr)
		return INFINITE;

	std::map<CTextureBase*, Uint>::iterator found = textures_locations_map.find(texture_);
	if (found != textures_locations_map.end())
		return found->second;

	return INFINITE;
}