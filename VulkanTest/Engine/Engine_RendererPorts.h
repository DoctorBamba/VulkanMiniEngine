#pragma once
#include "Engine_Renderer.h"

class CWindow;

namespace Engine
{
	class RendererWindowPort
	{
		struct TickGraphicsInfo
		{
			CFrameBuffer*	target;
			CGpuDrawTask*	draw_task;
		};

		public:
			std::vector<TickGraphicsInfo> m_FramesInfo;

			Uint						buffer_index;
			VkSemaphore					backbuffer_avalible_semaphore;
			VkSemaphore					render_finished_semaphore;

			const CWindow* p_WindowContext;
			
		public:
			
			RendererWindowPort(CWindow* window_);
			Void Display();
	};
}