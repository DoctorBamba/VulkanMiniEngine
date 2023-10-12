#pragma once
#include "FormatsMap.h"
#include "CDevice.h"
#include "CGpuTask.h"
#include "Buffers/CIntermidiate.h"


#include "FreeImage.h"
#pragma comment(lib, "FreeImage.lib")

/*
struct TextureFormats
{
	VkFormat				buffer_format;
	VkComponentSwizzle		component_mapping;
	PE_Uint					bytes;
};

static TextureFormats TF_UNDEFINE			= { VK_FORMAT_UNDEFINED,  ,  , 0u};

static TextureFormats TF_DEPTH_32F			= { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R8_UNORM, VK_FORMAT_D32_SFLOAT , 4u};
static TextureFormats TF_DEPTH_24_STENCIL8	= { VK_FORMAT_R24G8, VK_FORMAT_R8_UNORM, VK_FORMAT_D32_SFLOAT , 4u};
*/

class CTextureBase
{
	protected:
		enum class CreationWay
		{
			CREATE_FROM_DATA,
			CREATE_FROM_FILE,
			CREATE_FROM_BUFFER
		};

		enum class Dimension
		{
			Texture1D,
			Texture2D,
			Texture3D,
			Texture2DArray,
			Texture2DCube,
			Texture2DCubeArray
		};

	protected:
		std::string				m_Name;
		std::wstring			m_ImagePath; //Optional

		Uint16					m_Width;
		Uint16					m_Height;
		Uint16					m_Layers = 1;

		VkFormat				m_Format;
		VkImageLayout			m_Layout;

		Uint					m_RequiredSize;

		VkImage					p_Image;
		VkImageView				p_ImageView;

		CMemorySpace::BindingLocation m_BindingLocation;

		const CreationWay m_CreationWay;
		const Dimension	  m_Dimension;

		CVulkanDevice* const p_Device;
		CMemorySpace* const	p_MemorySpace;

	protected:
		CTextureBase(CVulkanDevice* device_) : m_CreationWay(CreationWay::CREATE_FROM_BUFFER), m_Dimension(Dimension::Texture2D), p_Device(device_), p_MemorySpace(nullptr) {}
		CTextureBase(CMemorySpace* memory_type_, Dimension dimantion, CreationWay creation_way_) : m_CreationWay(creation_way_), m_Dimension(dimantion), p_Device(memory_type_->GetDevice()), p_MemorySpace(memory_type_){}

		VkAccessFlagBits GetRequierdAccessFlag(VkImageLayout layout_);

		VkImageSubresourceRange GetSubresourceRange();

	public:

		virtual ~CTextureBase();

		Void CreateBuffer(Bool attachment_usage_, Pointer construct_data_ = nullptr, CGpuUploadTask* upload_task_ = nullptr);
		
		virtual Void CreateView() {};
		virtual Void Upload(Pointer construct_data_, CGpuUploadTask* upload_task_) {};

		Void Barrier(VkImageLayout new_layout_, CGpuTask* task_);
		Void Bind(VkDescriptorSet descriptor_set_, Uint binding_, Uint array_index_ = 0) const;
		
		Bool SupportMemoryType(Uint memory_type_);

		VkImage GetVkImage() { return p_Image; }
		VkImageView& GetBufferView() noexcept { return p_ImageView; };

		Uint GetWidth() const { return m_Width; }
		Uint GetHeight() const { return m_Height; }
		VkFormat GetFormat() const { return m_Format; }

		Uint GetRequiredSize() const { return m_RequiredSize; }

		VkImageLayout GetImageLayout() { return m_Layout; }

		VkImageMemoryBarrier BarrierTranslation(VkImageLayout new_layout_);

		Bool IsBoundable() { return m_CreationWay != CreationWay::CREATE_FROM_BUFFER; }
		Bool IsBaseOnBuffer() { return m_CreationWay == CreationWay::CREATE_FROM_BUFFER; }

};