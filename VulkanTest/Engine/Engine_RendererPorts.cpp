#include "Engine_RendererPorts.h"
#include "../CWindow.h"

Engine::RendererWindowPort::RendererWindowPort(CWindow* window_) : p_WindowContext(window_)
{
	renderer->Resize(window_->m_SwapChainInfo.width, window_->m_SwapChainInfo.height);

	//Create Frame buffers...

	const CRenderPass* render_pass = renderer->GetRenderPass().output;

	std::vector<CTexture2D*> back_textures = TexturesFromBackBuffers(	graphics->p_Device,
																		window_->m_SwapChainInfo.swap_chain, 
																		window_->m_SwapChainInfo.width, 
																		window_->m_SwapChainInfo.height, 
																		window_->m_SwapChainInfo.format);


	for (Uint i = 0; i < back_textures.size(); i++)
	{
		FrameInfo frame_info;
		frame_info.target		= new CFrameBuffer(graphics->GetVkDevice(), { back_textures.at(i) }, nullptr, render_pass);
		frame_info.draw_task	= new CGpuDrawTask(graphics->p_Device, graphics->p_CommandPool, i);

		m_FramesInfo.push_back(frame_info);
	}


	VkSemaphoreCreateInfo sempaphore_desc{};

	sempaphore_desc.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(graphics->GetVkDevice(), &sempaphore_desc, nullptr, &backbuffer_avalible_semaphore))
		throw std::runtime_error("Failed to create a semaphore!");

	sempaphore_desc.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(graphics->GetVkDevice(), &sempaphore_desc, nullptr, &render_finished_semaphore))
		throw std::runtime_error("Failed to create a semaphore!");
}

Void Engine::RendererWindowPort::Display()
{
	if (vkAcquireNextImageKHR(graphics->GetVkDevice(), p_WindowContext->m_SwapChainInfo.swap_chain, UINT64_MAX, backbuffer_avalible_semaphore, VK_NULL_HANDLE, &buffer_index) != VK_SUCCESS)
		throw std::runtime_error("RendererWindowPort::Present -> Synchonization Faild");

	FrameInfo& frame_info = m_FramesInfo.at(buffer_index);

	frame_info.draw_task->WaitAntilComplite();
	frame_info.draw_task->Reset();

	SetViewport(frame_info.draw_task->GetCommandBuffer(), 0, 0, 1024, 768);

	Engine::Draw_Event(frame_info.draw_task, frame_info.target, buffer_index);
	
	frame_info.draw_task->Execute(graphics->p_GraphicsQueue, backbuffer_avalible_semaphore, render_finished_semaphore);

	p_WindowContext->Present(buffer_index, render_finished_semaphore);
}