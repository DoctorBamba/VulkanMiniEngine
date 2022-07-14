#pragma once
#include "VulkanExtentions.h"
#include "Graphics/CFrameBuffer.h"
#include "Graphics/CGpuTask.h"
#include "Graphics/Buffers/CBuffer.h"
#include "Graphics/Buffers/CUniformBuffer.h"
#include "Graphics/CMemoryManager.h"
#include "Graphics/CDescriptorPool.h"

#define NOT_AVAILABLE 0xffffffff
#define MAXIMUM_UNIFORM_BUFFERS_PER_FRAME 1000000

//A list of layers name to use for debug/trace vulkan
static const std::vector<String> VulkanValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> VulkanDeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
};

static void* VulkanDeviceExtensionsFeatures()
{
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT* dynamice_state_features = new VkPhysicalDeviceExtendedDynamicStateFeaturesEXT;
	memset(dynamice_state_features, 0, sizeof(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT));
	dynamice_state_features->sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    dynamice_state_features->extendedDynamicState = true;

    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* descriptor_indexing_features = new VkPhysicalDeviceDescriptorIndexingFeaturesEXT;
	memset(descriptor_indexing_features, 0, sizeof(VkPhysicalDeviceDescriptorIndexingFeaturesEXT));
	dynamice_state_features->pNext = descriptor_indexing_features;
    descriptor_indexing_features->sType                                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    descriptor_indexing_features->shaderSampledImageArrayNonUniformIndexing  = VK_TRUE;
    descriptor_indexing_features->runtimeDescriptorArray                     = VK_TRUE;
    descriptor_indexing_features->descriptorBindingVariableDescriptorCount   = VK_TRUE;
    descriptor_indexing_features->descriptorBindingPartiallyBound            = VK_TRUE;
	
	return (void*)dynamice_state_features;
}


struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	presentModes;
};

class CGraphics
{
	public:
		VkInstance		 p_Instance;
		VulkanDevice*	 p_Device;
		VkQueue			 p_GraphicsQueue;
		VkQueue			 p_PresentQueue;
		VkCommandPool	 p_CommandPool;

		struct DeviceMemory
		{
			CMemorySpace* uniform_space;
			CMemorySpace* local_space;
		}m_DeviceMemory;

		Uint m_MainQueueFamilyIndex;
		Uint m_FramesCount = 3;

		std::vector<VkDescriptorPool> m_DescriptorPools;

	public:
		CGraphics();
		~CGraphics();
		Void PrintExtentionsList();
		Void CreateInstance();
		Void PickPhysicalDevice();
		Void CreateDevice();
		Void AllocateMemory();
		Void CreateCommandPool();
		Void CreateDescriptorPool();

		//Support Chacking...
		Bool ChackDeviceRequirements(VkPhysicalDevice physical_device_);//Chack if an goven device meets all Engine/Application requirements
		Bool ChackDefaultSwapChinSupport(VkSurfaceKHR surface_);//Chack if the choosen PhysicalDevice support a given surface

		//Swap Chain Properities...
		SwapChainSupportDetails GetSwapChainSupport(VkSurfaceKHR surface_);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats_);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes_);

		VkDevice GetVkDevice() { return p_Device->device; }
};


Uint FindGraphicsFamily(VkPhysicalDevice physical_device_, VkQueueFlagBits queue_types_);