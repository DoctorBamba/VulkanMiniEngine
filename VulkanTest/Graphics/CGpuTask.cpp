#include "CGpuTask.h"
#include "Buffers/CIntermidiate.h"

CGpuTask::CGpuTask(CVulkanDevice* device_, VkCommandPool command_pool_) : p_Device(device_)
{
	VkFenceCreateInfo fence_desc{};
	fence_desc.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (vkCreateFence(p_Device->device, &fence_desc, nullptr, &p_Fence) != VK_SUCCESS)
	{
		throw std::runtime_error("CGpuTask::CGpuTask -> Failed to create fence");
		return;
	}

	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool			= command_pool_;
	alloc_info.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount	= 1;

	if (vkAllocateCommandBuffers(p_Device->device, &alloc_info, &p_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("CGpuTask::CGpuTask -> Failed to allocate command buffer!");
		return;
	}

	m_InRecording  = false;
	m_TaskSubmited = false;
}

CGpuTask::~CGpuTask()
{
	vkDestroyFence(p_Device->device, p_Fence, nullptr);
}


Void CGpuTask::Reset()
{
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType			= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags			= 0;
	begin_info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(p_CommandBuffer, &begin_info) != VK_SUCCESS)
		throw std::runtime_error("CGpuTask::Reset -> Command buffer Reset Failed!");
		
	m_InRecording = true;
}

Void CGpuTask::Close()
{
	if (vkEndCommandBuffer(p_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("CGpuTask::Close -> Failed to close command buffer");
		return;
	}
		
	m_InRecording = false;
}

Void CGpuTask::Execute(VkQueue queue_, VkSemaphore wait_semaphore_, VkSemaphore signal_semaphore_)
{
	Close();

	vkResetFences(p_Device->device, 1, &p_Fence);

	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submit_info{};
	submit_info.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount		= 1;
	submit_info.pCommandBuffers			= &p_CommandBuffer;

	if (wait_semaphore_ != VK_NULL_HANDLE)
	{
		////wait for wait_semaphore_ before attachment output stage...
		submit_info.pWaitSemaphores		= &wait_semaphore_;
		submit_info.waitSemaphoreCount	= 1;
		submit_info.pWaitDstStageMask	= wait_stages;
	}

	if (wait_semaphore_ != VK_NULL_HANDLE)
	{
		submit_info.pSignalSemaphores	 = &signal_semaphore_;
		submit_info.signalSemaphoreCount = 1;
	}

	if (vkQueueSubmit(queue_, 1, &submit_info, p_Fence) != VK_SUCCESS)
		throw std::runtime_error("CGpuTask::Execute -> Submition failed!");

	m_TaskSubmited = true;
}

Void CGpuTask::WaitAntilComplite()
{
	if(m_TaskSubmited)
		vkWaitForFences(p_Device->device, 1, &p_Fence, true, UINT64_MAX);
}

//Upload Task...

CGpuUploadTask::~CGpuUploadTask()
{
	for (Uint i = 0; i < m_Intermidiates.size(); i++)
		delete m_Intermidiates.at(i);

	m_Intermidiates.clear();
}