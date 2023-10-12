#pragma once
#include "CTextureBase.h"

class CTextureCube : public CTextureBase
{
	public:
		using CTextureBase::CTextureBase;

		CTextureCube(CMemorySpace* memory_space_, Uint16 width_, Uint16 height_, VkFormat format_, Bool attachment_usage_, Pointer construct_data_ = nullptr, CGpuUploadTask* upload_task_ = nullptr, std::string name_ = "TextureCube");
		
		Void CreateView() override;
		Void Upload(Pointer construct_data_, CGpuUploadTask* upload_task_) override;
};
