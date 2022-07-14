#include "Engine_Graphics.h"

namespace Engine
{
	CGraphics* graphics;
	VkDescriptorSetLayout	static_descriptor_set_layout;
	VkDescriptorSet			static_descriptor_set;

	ResourcesMenager* resource_manager;
}


Void Engine::SetViewport(VkCommandBuffer command_buffer_, Float x_, Float y_, Float width_, Float height_)
{
	VkViewport viewport{};
	viewport.x			= x_;
	viewport.y			= y_;
	viewport.width		= width_;
	viewport.height		= height_;
	viewport.minDepth	= 0.0f;
	viewport.maxDepth	= 1.0f;

	vkCmdSetViewport(command_buffer_, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { (Uint)width_, (Uint)height_ };

	vkCmdSetScissor(command_buffer_, 0, 1, &scissor);
}