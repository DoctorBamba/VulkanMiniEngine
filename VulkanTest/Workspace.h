#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <list>
#include <map>
#include <string>
#include <array>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "vulkan-1.lib")


#pragma warning(disable : 26812)

#include "LinearMath/PE_LinearMath.h"
#include "Directory.h"

#define array_size(a) (sizeof(a) / sizeof(*a))

struct VulkanDevice
{
	VkDevice					device;
	VkPhysicalDevice			physical_device;
	VkPhysicalDeviceProperties	properties;
};