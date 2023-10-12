#pragma once
#include "CTextureBase.h"

class CTexture2DArray : public CTextureBase
{
	public:
		using CTextureBase::CTextureBase;

		CTexture2DArray(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, Uint count_, VkFormat format_, Bool attachment_usage_, Pointer construct_data_ = nullptr, CGpuUploadTask* upload_task_ = nullptr, std::string name_ = "TextureCube");
		
		Void CreateView() override;
		Void Upload(Pointer construct_data_, CGpuUploadTask* upload_task_) override;
};
