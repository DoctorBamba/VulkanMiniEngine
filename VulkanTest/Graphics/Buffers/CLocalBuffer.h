#pragma once
#include "CIntermidiate.h"

class CLocalBuffer : public CIndependentBuffer
{
	public:
		CLocalBuffer(CVulkanDevice* device_, Uint allocate_size_, Uint usage_flag_, CGpuUploadTask* upload_task_ = nullptr, Pointer initional_data_ = nullptr)
			: CIndependentBuffer(device_,
					  allocate_size_, 
					  usage_flag_ | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false) 
		{
			if (initional_data_ == nullptr)
				return;

			if (upload_task_ == nullptr)
			{
				throw std::runtime_error("CLocalBuffer Error -> Initional data is avalible but the given GPU-Task is nullptr!");
				return;
			}

			Upload(upload_task_, allocate_size_, initional_data_);
		}


		Void Upload(CGpuTask* upload_task_, CIntermidiateBuffer* uploader_, Uint bytes_offset_ = 0)
		{
			if (!upload_task_->InRecording())
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The Upload Task must be in recording!");
				return;
			}

			if (uploader_->GetUsage() != CIntermidiateBuffer::Usage::Upload)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The intermidaite buffer usage must be ::Usage::Upload!");
				return;
			}

			if (uploader_->GetSize() > m_Size - bytes_offset_)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> In this meshod the Upload buffer's size must be smaller then the buffer's size minus the offset.");
				return;
			}

			if (uploader_ == nullptr)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The Uploader buffer is nullptr!");
				return;
			}

			VkBufferCopy copy_region{};
			copy_region.srcOffset = 0;			// Optional
			copy_region.dstOffset = bytes_offset_;	// Optional
			copy_region.size	  = uploader_->GetSize();
			vkCmdCopyBuffer(upload_task_->GetCommandBuffer(), uploader_->GetVkBuffer(), p_Buffer, 1, &copy_region);
		}

		Void Upload(CGpuUploadTask* upload_task_, Uint data_size_, Pointer data_, Uint offset_ = 0)
		{
			CIntermidiateBuffer* uploader = new CIntermidiateBuffer(p_Device, data_size_, CIntermidiateBuffer::Usage::Upload);

			Pointer map_data = uploader->Map();
			memcpy(map_data, data_, data_size_);
			uploader->Unmap();

			Upload(upload_task_, uploader, offset_);

			upload_task_->m_Intermidiates.push_back(uploader);//Need this to remember that after the task complite this uploader buffer should to remove.
		}

		Void Download(CGpuTask* download_task_, CIntermidiateBuffer* downloader_, Uint offset_ = 0)
		{
			if (!download_task_->InRecording())
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The Upload Task must be in recording!");
				return;
			}

			if (downloader_->GetUsage() != CIntermidiateBuffer::Usage::Download)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The intermidaite buffer usage must be ::Usage::Upload!");
				return;
			}

			if (downloader_->GetSize() > m_Size - offset_)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> In this meshod the intermidaite buffer's size must be smaller then the buffer's size minus the offset.");
				return;
			}

			if (downloader_ == nullptr)
			{
				throw std::runtime_error("CLocalBuffer::Upload Error -> The Downloader buffer is nullptr!");
				return;
			}

			VkBufferCopy copy_region{};
			copy_region.srcOffset = 0; // Optional
			copy_region.dstOffset = 0; // Optional
			copy_region.size	  = downloader_->GetSize();
			vkCmdCopyBuffer(download_task_->GetCommandBuffer(), p_Buffer, downloader_->GetVkBuffer(), 1, &copy_region);
		}

		Byte* Download(VkQueue queue_, VkCommandPool command_pool_)
		{
			Byte* data_out = new Byte[m_Size];
			CIntermidiateBuffer* downloader = new CIntermidiateBuffer(p_Device, m_Size, CIntermidiateBuffer::Usage::Download);

			CGpuDownloadTask* download_task = new CGpuDownloadTask(p_Device, command_pool_);
			download_task->Reset();

			Download(download_task, downloader);

			download_task->m_Intermidiates.push_back(downloader);//Need this to remember that after the task complite this uploader buffer should to remove.

			download_task->Execute(queue_);
			download_task->WaitAntilComplite();//Wait for copy to complite

			Pointer map_data = downloader->Map();
			memcpy(data_out, map_data, m_Size);
			downloader->Unmap();

			delete download_task;//Delete the task and the intermidiate buffer

			return data_out;
		}
};