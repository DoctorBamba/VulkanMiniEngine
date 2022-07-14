#pragma once
#include "CPcInput.h"
#include "CGraphics.h"
#include "Engine/Engine_Events.h"
#include "Engine/Engine_RendererPorts.h"

class CWindow;
namespace Engine
{
	Void CreateWindowRendererPort(CWindow* window_);
}

extern std::map<GLFWwindow*, CWindow*> GLFW_WINDOWS_MAP;

class CWindow
{
	friend Engine::RendererWindowPort;

	/*
	static void KeyCallback(GLFWwindow* window_, int key_, int scancode_, int action_, int mods_)
	{
		GLFW_WINDOWS_MAP.at(window_)->p_InputState->UpdateKeyState(key_, scancode_, action_, mods_);
	}

	static void CursorPositionCallback(GLFWwindow* window_, double xpos_, double ypos_)
	{
		GLFW_WINDOWS_MAP.at(window_)->p_InputState->UpdateCursorPosition(xpos_, ypos_);
	}

	static void MouseCallback(GLFWwindow* window_, double xpos_, double ypos_)
	{
		GLFW_WINDOWS_MAP.at(window_)->p_InputState->UpdateCursorPosition(xpos_, ypos_);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			popup_menu();
	}
	*/

	struct SwapChainInfo
	{
		Uint16		width;
		Uint16		height;
		VkFormat		format;
		VkSwapchainKHR	swap_chain;
	};

	protected:
		GLFWwindow*							p_Window;
		VkSurfaceKHR						p_Surface;
		Engine::RendererWindowPort*			p_RendererPort;

		SwapChainInfo						m_SwapChainInfo;

		CGraphics* const p_GraphicsContext;
	public:
		CWindow(Int16 width_, Int16 height_, std::string title_, CGraphics* graphics_) : p_GraphicsContext(graphics_)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			p_Window = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
			if (p_Window == nullptr)
				throw std::runtime_error("CWindow Error : Faild to Create The Window!");

			CreateSurface(graphics_);
			CreateSwapChain(graphics_);
			
			p_RendererPort = new Engine::RendererWindowPort(this);
		}

		Void CreateSurface(CGraphics* graphics_)
		{
			if (glfwCreateWindowSurface(graphics_->p_Instance, p_Window, nullptr, &p_Surface) != VK_SUCCESS)
				throw std::runtime_error("Failed to create window surface!");
		}

		Void CreateSwapChain(CGraphics* graphics_)
		{
			if (!graphics_->ChackDefaultSwapChinSupport(p_Surface))
				throw std::runtime_error("Creation of Difualt Swap Chain for this window surface not supported by the Physical Device or The Grhaphic Queue");


			SwapChainSupportDetails swap_chain_support = graphics_->GetSwapChainSupport(p_Surface);

			Int width, height;
			glfwGetFramebufferSize(p_Window, &width, &height);
			width  = std::clamp((uint32_t)width, swap_chain_support.capabilities.minImageExtent.width, swap_chain_support.capabilities.maxImageExtent.width);
			height = std::clamp((uint32_t)width, swap_chain_support.capabilities.minImageExtent.height, swap_chain_support.capabilities.maxImageExtent.height);

			VkSurfaceFormatKHR surface_format   = graphics_->ChooseSwapSurfaceFormat(swap_chain_support.formats);
			VkPresentModeKHR   present_mode		= graphics_->ChooseSwapPresentMode(swap_chain_support.presentModes);
			VkExtent2D		   extent			= { (uint32_t)width, (uint32_t)height };


			VkSwapchainCreateInfoKHR swap_chin_desc{};
			swap_chin_desc.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swap_chin_desc.surface					= p_Surface;
			swap_chin_desc.minImageCount			= swap_chain_support.capabilities.minImageCount + 1;
			swap_chin_desc.imageFormat				= surface_format.format;
			swap_chin_desc.imageColorSpace			= surface_format.colorSpace;
			swap_chin_desc.imageExtent				= extent;
			swap_chin_desc.imageArrayLayers			= 1;
			swap_chin_desc.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swap_chin_desc.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			swap_chin_desc.queueFamilyIndexCount	= 0;
			swap_chin_desc.pQueueFamilyIndices		= nullptr;
			swap_chin_desc.preTransform				= swap_chain_support.capabilities.currentTransform;
			swap_chin_desc.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swap_chin_desc.presentMode				= present_mode;
			swap_chin_desc.clipped					= VK_TRUE;
			swap_chin_desc.oldSwapchain				= VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(graphics_->GetVkDevice(), &swap_chin_desc, nullptr, &m_SwapChainInfo.swap_chain) != VK_SUCCESS)
				throw std::runtime_error("Failed to create swap chain!");

			
			m_SwapChainInfo.width	= width;
			m_SwapChainInfo.height	= height;
			m_SwapChainInfo.format	= surface_format.format;
		}

		virtual ~CWindow()
		{
			delete p_RendererPort;

			if (m_SwapChainInfo.swap_chain != nullptr)
				vkDestroySwapchainKHR(p_GraphicsContext->GetVkDevice(), m_SwapChainInfo.swap_chain, nullptr);

			if (p_Surface != nullptr)
				vkDestroySurfaceKHR(p_GraphicsContext->p_Instance, p_Surface, nullptr);

			if(p_Window != nullptr)
				glfwDestroyWindow(p_Window);
		}

		Void Present(Uint backbuffer_index_, VkSemaphore wait_semphore_) const
		{
			VkPresentInfoKHR present_info{};
			present_info.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores	= &wait_semphore_;
			present_info.swapchainCount		= 1;
			present_info.pSwapchains		= &m_SwapChainInfo.swap_chain;
			present_info.pImageIndices		= &backbuffer_index_;

			if (vkQueuePresentKHR(p_GraphicsContext->p_GraphicsQueue, &present_info) != VK_SUCCESS)
				throw std::runtime_error("Present Error!");
		}

		Int Loop()
		{
			while (!glfwWindowShouldClose(p_Window))
			{
				glfwPollEvents();

				p_RendererPort->Display();
			}

			return 0;
		}

		//GLFW Input controll...

		Coordinates GetClientDimantions()
		{
			int width, height;
			glfwGetFramebufferSize(p_Window, &width, &height);
			return Coordinates(width, height);
		}

		Bool IsKeyPressed(int key_) { return glfwGetKey(p_Window, key_) == GLFW_PRESS; }

		Coordinates GetCursorPosition()
		{
			double cursor_pos[2];
			glfwGetCursorPos(p_Window, &(cursor_pos[0]), &(cursor_pos[1]));
			return Coordinates(cursor_pos[0], cursor_pos[1]);
		}

		Void SetCursorPosition(Coordinates pos_)
		{
			glfwSetCursorPos(p_Window, (double)pos_[0], (double)pos_[1]);
		}
};