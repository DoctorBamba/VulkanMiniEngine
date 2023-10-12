#pragma once
#include "../Engine_GpuStructures.h"
#include "../Engine_BasicSamplers.h"
#include "../../Graphics/CAllocator.h"
#include "../../Graphics/CTexture2D.h"
#include "../../Graphics/Buffers/CStorgeBuffer.h"
#include "../../Scene/CMaterial.h"

#define ENGINE_MAXIMUM_MATERIALS_COUNT		100000
#define ENGINE_OBJECT_TEXTURES_BLOCK_SIZE	6//Each object diffuse, specular, emmisive, shini, reflection, 
#define ENGINE_MAXIMUM_TEXTURES_COUNT		(ENGINE_MAXIMUM_MATERIALS_COUNT * ENGINE_OBJECT_TEXTURES_BLOCK_SIZE)

#define UNUSED 0xffffffff

class CPbrMaterial;

namespace Engine
{
	class ResourcesMenager
	{
		private:
			VkDescriptorSetLayout		static_descriptor_set_layout;
			VkDescriptorPool			static_descriptor_pool;
			VkDescriptorSet				static_descriptor_set;

			CStorgeBuffer<GpuPropertiesBlockStruct>*	materials_buffer;
			CBlocksAllocator*							materials_allocator;
			std::map<CMaterial*, Uint>					materials_locations_map;

			CBlocksAllocator*							textures_allocator;
			std::map<CTextureBase*, Uint>				textures_locations_map;

			CVulkanDevice* const p_Device;

		private:
			CTexture2D* unknow_texture;

		public:

			Void CreateStaticDescriptorSet(Uint textures_count_limit_);
			ResourcesMenager(CVulkanDevice* device_, Uint materials_count_limit_, Uint textures_count_limit_);

			Void UploadMaterialsPacket(std::vector<CMaterial*> packet_, CGpuUploadTask* upload_task_);
			Uint GetMaterialLocation(CMaterial* data_block_);
			CStorgeBuffer<GpuPropertiesBlockStruct>* GetDataBlocksBuffer() { return materials_buffer; }
			
			
			Void AddTexturesPacket(std::vector<CTextureBase*> textures_);
			Uint GetTextureLocation(CTextureBase* texture_);
		
	
			VkDescriptorSetLayout GetDescriptorLayout() { return static_descriptor_set_layout; }
			VkDescriptorSet GetDescriptorSet() { return static_descriptor_set; }
	};

	extern ResourcesMenager* resource_manager;
}