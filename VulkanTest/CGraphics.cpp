#include "CGraphics.h"

CGraphics::CGraphics() : p_Device(new VulkanDevice)
{
    PrintExtentionsList();
    CreateInstance();
    PickPhysicalDevice();
    CreateDevice();
    AllocateMemory();
    CreateCommandPool();
    CreateDescriptorPool();
}

Bool CheckValidationLayerSupport()
{
    Uint layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);//Only get the layer count.

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    
    //Orgenize layers in map for fast sertching...
    std::map<std::string, VkLayerProperties> available_layers_map;
    for (Uint i = 0; i < layer_count ; i++)
        available_layers_map.insert(std::pair<std::string, VkLayerProperties>(available_layers.at(i).layerName, available_layers.at(i)));

    //Sertch...
    for (Uint i = 0 ; i < VulkanValidationLayers.size() ; i ++)//Pass all list
    {
        auto it = available_layers_map.find(VulkanValidationLayers.at(i));//Sercth layouy by name in map
        if (it == available_layers_map.end())
            return false;
    }

    return true;
}

Void CGraphics::CreateInstance() 
{
    if (!CheckValidationLayerSupport())
    {
        throw std::runtime_error("One or More Validation Layer In the Validation List Not Supported!");
        return;
    }

    Uint    glfw_extentions_count = 0;
    String* glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extentions_count);

    VkApplicationInfo app_info{};
    app_info.sType                  = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName       = "Hello Triangle";
    app_info.applicationVersion     = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName            = "No Engine";
    app_info.engineVersion          = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion             = VK_API_VERSION_1_1;

    VkInstanceCreateInfo create_info{};
    create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo        = &app_info;
    create_info.enabledExtensionCount   = glfw_extentions_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount       = VulkanValidationLayers.size();
    create_info.ppEnabledLayerNames     = VulkanValidationLayers.data();

    VkResult result = vkCreateInstance(&create_info, nullptr, &p_Instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Vulkan Instance Failed to Create!");
        return;
    }
}



SwapChainSupportDetails GetSwapChainSupport(VkPhysicalDevice physical_device_, VkSurfaceKHR surface_)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface_, &details.capabilities);

    Uint format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_, &format_count, details.formats.data());
    }

    Uint present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface_, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface_, &present_mode_count, details.presentModes.data());
    }

    return details;
}

Bool CGraphics::ChackDeviceRequirements(VkPhysicalDevice physical_device_)
{
    //Cheak device properties...
    VkPhysicalDeviceProperties2 device_properties{};
    device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(physical_device_, &device_properties);

    if (device_properties.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        return false;

    //Check Exeptions Support...
    Uint extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(VulkanDeviceExtensions.begin(), VulkanDeviceExtensions.end());

    for (const auto& extension : available_extensions)
        required_extensions.erase(extension.extensionName);

    if (!required_extensions.empty())
        return false;

    p_Device->properties = device_properties.properties;

    return true;
}

Void CGraphics::PickPhysicalDevice()
{
    Uint device_count = 0;
    vkEnumeratePhysicalDevices(p_Instance, &device_count, nullptr);

    if (device_count == 0)
    {
        throw std::runtime_error("CreateDevice Failed - Not found GPU id found");
        return;
    }

    std::vector<VkPhysicalDevice> physical_devices(device_count);
    vkEnumeratePhysicalDevices(p_Instance, &device_count, physical_devices.data());
    
    //Choose a device that support the application requirements...
    p_Device->physical_device = VK_NULL_HANDLE;
    for (Uint i = 0; i < device_count; i++)
    {
        if (ChackDeviceRequirements(physical_devices.at(i)))
        {
            p_Device->physical_device = physical_devices.at(i);
            break;
        }
    }

    if (p_Device->physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
        return;
    }


    //Find a Queue family that support Graphics/3D Engine
    m_MainQueueFamilyIndex = FindGraphicsFamily(p_Device->physical_device, VK_QUEUE_GRAPHICS_BIT);
}

Uint FindGraphicsFamily(VkPhysicalDevice physical_device_, VkQueueFlagBits queue_types_)
{
    //Serche for Physical Queues Families...
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count, nullptr);//Only get the number

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count, queue_families.data());

    Uint family_index = NOT_AVAILABLE;

    for (Uint i = 0; i < queue_family_count; i++)
    {
        if (queue_families.at(i).queueFlags & queue_types_)
        {
            family_index = i;
            break;
        }
    }

    if (family_index == NOT_AVAILABLE)
        throw std::runtime_error("Not found a family that support the given Queue-Type");

    return family_index;
}

Uint FindSupportedMemoryType(VkPhysicalDevice physical_device_, Uint memory_type_flag_, Uint addition_type_requirments_)
{
	VkPhysicalDeviceMemoryProperties memory_prop;
	vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_prop);

	for (Uint i = 0; i < memory_prop.memoryTypeCount; i++)
	{
		Uint bit_mask = (1u << i);
		if ((memory_type_flag_ & bit_mask) && ((memory_prop.memoryTypes[i].propertyFlags & addition_type_requirments_) == addition_type_requirments_))
			return i;
	}

	throw std::runtime_error("FindMemoryTypeIndex -> Not found memory with memory type that support he requirment of this buffer!");
	return 0;
}


SwapChainSupportDetails CGraphics::GetSwapChainSupport(VkSurfaceKHR surface_)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_Device->physical_device, surface_, &details.capabilities);

    Uint format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(p_Device->physical_device, surface_, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(p_Device->physical_device, surface_, &format_count, details.formats.data());
    }

    Uint present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(p_Device->physical_device, surface_, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(p_Device->physical_device, surface_, &present_mode_count, details.presentModes.data());
    }

    return details;
}

//The Next function cheak whether creation of Swap Chian for the given surface is supported
Bool CGraphics::ChackDefaultSwapChinSupport(VkSurfaceKHR surface_)
{
    //Cheak if the graphics queue also support presentation...
    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(p_Device->physical_device, m_MainQueueFamilyIndex, surface_, &support);
    if (!support)
        return false;

    return true;
}

VkSurfaceFormatKHR CGraphics::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats_) 
{
    for (const auto& available_format : available_formats_)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return available_format;
    }

    return available_formats_[0];
}

VkPresentModeKHR CGraphics::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes_)
{
    for (const auto& available_present_mode : available_present_modes_) 
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return available_present_mode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

Void CGraphics::CreateDevice()
{
	Float queue_priority = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queue_descs;
	VkDeviceQueueCreateInfo queue_desc = {};

	queue_desc.sType			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_desc.queueFamilyIndex = m_MainQueueFamilyIndex;
	queue_desc.queueCount		= 1;
	queue_desc.pQueuePriorities = &queue_priority;
	queue_descs.push_back(queue_desc);

	VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy   = true;
    device_features.geometryShader      = true;
    device_features.tessellationShader  = true;
    device_features.fillModeNonSolid    = true;

	VkDeviceCreateInfo deivce_desc{};
	deivce_desc.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deivce_desc.pQueueCreateInfos       = queue_descs.data();
	deivce_desc.queueCreateInfoCount    = queue_descs.size();
	deivce_desc.pEnabledFeatures        = &device_features;
	deivce_desc.enabledExtensionCount   = VulkanDeviceExtensions.size();
	deivce_desc.ppEnabledExtensionNames = VulkanDeviceExtensions.data();
	deivce_desc.enabledLayerCount       = VulkanValidationLayers.size();
	deivce_desc.ppEnabledLayerNames     = (VulkanValidationLayers.size()) ? VulkanValidationLayers.data() : nullptr;
    deivce_desc.pNext                   = VulkanDeviceExtensionsFeatures();

	if (vkCreateDevice(p_Device->physical_device, &deivce_desc, nullptr, &p_Device->device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create device!");

	vkGetDeviceQueue(p_Device->device, m_MainQueueFamilyIndex, 0, &p_GraphicsQueue);

    LoadExtentionsFuncPointers(p_Device->device);

}

Void CGraphics::AllocateMemory()
{
    m_DeviceMemory.uniform_space    = new CMemorySpace(p_Device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, __128MB);
    m_DeviceMemory.local_space      = new CMemorySpace(p_Device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, __128MB);
    m_DeviceMemory.uniform_space->Map();//Map uniform memory becuse we need to update this memory repadly

}

Void CGraphics::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex	= m_MainQueueFamilyIndex;
	poolInfo.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			
	if (vkCreateCommandPool(p_Device->device, &poolInfo, nullptr, &p_CommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("CGraphics::CreateCommandPool -> Failed to create command pool!");
	}
}

Void CGraphics::CreateDescriptorPool()
{
    m_DescriptorPools.resize(m_FramesCount);

    //Create descriptor pool for evrey frame...
    for (Uint i = 0; i < m_FramesCount; i++)
    {
        VkDescriptorPoolSize uniforms_size{};
        uniforms_size.type                      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniforms_size.descriptorCount           = 256 * m_FramesCount;

        VkDescriptorPoolSize dynamic_uniforms_size{};
        dynamic_uniforms_size.type             = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        dynamic_uniforms_size.descriptorCount  = 16 * m_FramesCount;;

        VkDescriptorPoolSize images_size{};
        images_size.type                        = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        images_size.descriptorCount             = 256 * m_FramesCount;

        VkDescriptorPoolSize pool_sizes[] = { uniforms_size, dynamic_uniforms_size, images_size };

        VkDescriptorPoolCreateInfo pool_desc{};
        pool_desc.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_desc.poolSizeCount = array_size(pool_sizes);
        pool_desc.pPoolSizes    = pool_sizes;
        pool_desc.maxSets       = MAXIMUM_DESCRIPTOR_SETS_PER_POOL;

        if (vkCreateDescriptorPool(p_Device->device, &pool_desc, nullptr, &m_DescriptorPools.at(i)) != VK_SUCCESS)
        {
            throw std::runtime_error("CDescriptorSet::Constructor -> Failed to create the descriptor pool!");
            return;
        }
    }
}

Void CGraphics::PrintExtentionsList()
{
    Uint extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);//Only for get extentions count

    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    std::cout << "Available Extensions:\n";

    for (Uint i = 0; i < extension_count; i++)
        std::cout << '\t' << extensions.at(i).extensionName << '\n';
}

CGraphics::~CGraphics()
{
    if (p_Device != nullptr)
        vkDestroyDevice(p_Device->device, nullptr);

    if (p_Instance != nullptr)
        vkDestroyInstance(p_Instance, nullptr);
}