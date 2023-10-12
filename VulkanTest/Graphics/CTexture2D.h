#pragma once
#include "CTextureBase.h"

class CTexture2D : public CTextureBase
{
	friend std::vector<CTexture2D*> TexturesFromBackBuffers(CVulkanDevice* device_, VkSwapchainKHR swap_chain_, Uint16 buffers_width_, Uint16 buffer_height_, VkFormat buffers_format_);

	public:
		using CTextureBase::CTextureBase;

		CTexture2D(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, VkFormat format_, Bool attachment_usage_, Pointer construct_data_ = nullptr, CGpuUploadTask* upload_task_ = nullptr, std::string name_ = "Texture2D");
		CTexture2D(CMemorySpace* memory_space_, std::wstring image_directory_, CGpuUploadTask* upload_task_);

		Void CreateView() override;
		Void Upload(Pointer construct_data_, CGpuUploadTask* upload_task_) override;
};

std::vector<CTexture2D*> TexturesFromBackBuffers(CVulkanDevice* device_, VkSwapchainKHR swap_chain_, Uint16 buffers_width_, Uint16 buffer_height_, VkFormat buffers_format_);