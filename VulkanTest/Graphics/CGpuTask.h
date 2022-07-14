#pragma once
#include "../Workspace.h"

class CGpuTask
{
	protected:
		VkCommandBuffer p_CommandBuffer;
		VkFence			p_Fence;
		Bool			m_InRecording;
		Bool			m_TaskSubmited;

		VulkanDevice* const	p_DeviceContext;

	public:
		CGpuTask(VulkanDevice* device_, VkCommandPool command_pool_);

		virtual ~CGpuTask();

		Void Reset();
		Void Close();
		Void Execute(VkQueue queue_, VkSemaphore wait_semaphore_ = VK_NULL_HANDLE, VkSemaphore signal_semaphore_ = VK_NULL_HANDLE);
		Void WaitAntilComplite();

		VkCommandBuffer GetCommandBuffer() { return p_CommandBuffer; }

		Bool InRecording() { return m_InRecording; }

		VulkanDevice* GetDevice() { return p_DeviceContext; }
};

class CLocalBuffer;
class CIntermidiateBuffer;
class CTexture2D;
class CTextureCube;

class CGpuUploadTask : public CGpuTask
{
	friend CLocalBuffer;
	friend CIntermidiateBuffer;
	friend CTexture2D;
	friend CTextureCube;

	private:
		std::vector<CIntermidiateBuffer*> m_Intermidiates;//Save a list of intermidaite buffer for remove they after task complite.

	public:
		using CGpuTask::CGpuTask;

		virtual ~CGpuUploadTask();

};


typedef CGpuUploadTask CGpuDownloadTask;

class CGpuDrawTask : public CGpuTask
{
	private:
		const Uint frame_index;

	public:
		CGpuDrawTask(VulkanDevice* device_, VkCommandPool command_pool_, Uint frame_index_) :
			CGpuTask(device_, command_pool_), frame_index(frame_index_) {}

		Uint GetFrameIndex() { return frame_index; }
};