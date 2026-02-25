
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeRenderUtils.h"


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
	VkDebugUtilsMessageTypeFlagsEXT Type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{

#ifdef LE_DEBUG

	LeChar Buf[4096];

	sprintf(Buf,
		"Debug callback: %s\n\t Severity %s\n\t Type %s\n\t Objects:",
		pCallbackData->pMessage, GetVkDebugUtilsMessageSeverityStr(Severity), GetVkDebugUtilsMessageType(Type));

	for (uint32 i = 0; i < pCallbackData->objectCount; i++)
	{
		LeChar Buf2[256];
		sprintf(Buf2, "\n\t\t%llx", pCallbackData->pObjects[i].objectHandle);
		strcat(Buf, Buf2);
	}

	switch (Severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LE_DEBUG_ERROR(Buf);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LE_DEBUG_WARN(Buf);
		break;
	default:
		LE_DEBUG_MESSAGE(Buf);
		break;
	}


#endif

	return VK_FALSE;  // The calling function should not be aborted
}


LeVulkanCore::LeVulkanCore()
{
}


LeVulkanCore::~LeVulkanCore()
{
	// Destroy the render queue
	m_queue.Destroy();

	// Destroy command pool
	vkDestroyCommandPool(m_device, m_cmdBufPool, NULL);

	// Destroy image views

	for (VkImageView& ImageView : m_imageViews)
		vkDestroyImageView(m_device, ImageView, NULL);
	LE_DEBUG_MESSAGE("Image views destroyed");

	// Destroy the swap chain

	vkDestroySwapchainKHR(m_device, m_swapChain, NULL);
	LE_DEBUG_MESSAGE("Swap chain destroyed");

	// Destroy logical device

	vkDestroyDevice(m_device, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");

	// Clear physical devices

	m_physicalDevices.DeInit();

	// Destroy surface

	PFN_vkDestroySurfaceKHR vkDestroySurface = VK_NULL_HANDLE;
	vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
	if (!vkDestroySurface)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroySurfaceKHR(m_instance, m_surface, NULL);
	LE_DEBUG_MESSAGE("Window surface destroyed");

	// Destroy debug utils messenger

	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
	vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!vkDestroyDebugUtilsMessenger)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroyDebugUtilsMessenger(m_instance, m_debugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");

	// Destroy instance

	vkDestroyInstance(m_instance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
}


void LeVulkanCore::Init(const char* pAppName, GLFWwindow* pWindow)
{
	CreateInstance(pAppName);
	CreateDebugCallback();
	CreateSurface(pWindow);
	m_physicalDevices.Init(m_instance, m_surface);
	m_queueFamily = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
	CreateDevice();
	CreateSwapChain();
	CreateCommandBufferPool();
	m_queue.Init(m_device, m_swapChain, m_queueFamily, 0);
}

void LeVulkanCore::CreateInstance(const char* pAppName)
{

	const char* Layers[] =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	const char* Extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#if defined (_WIN32)
		"VK_KHR_win32_surface",
#endif
#if defined (__APPLE__)
		"VK_MVK_macos_surface",
#endif
#if defined (__linux__)
		"VK_KHR_xcb_surface",
#endif
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	};



	VkApplicationInfo AppInfo =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = pAppName,
		.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.pEngineName = "LeEngine",
		.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,				// reserved for future use. Must be zero
		.pApplicationInfo = &AppInfo,
		.enabledLayerCount = ARRAY_SIZE(Layers),
		.ppEnabledLayerNames = Layers,
		.enabledExtensionCount = ARRAY_SIZE(Extensions),
		.ppEnabledExtensionNames = Extensions
	};


	VkResult res = vkCreateInstance(&CreateInfo, NULL, &m_instance);
	LE_CHECK_VK_RESULT(res, "Create instance");

	LE_DEBUG_MESSAGE("Vulkan instance created");

}



void LeVulkanCore::CreateDebugCallback()
{
	VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo{};
	MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	MessengerCreateInfo.messageSeverity =
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT	|
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	MessengerCreateInfo.pfnUserCallback = DebugCallback;
	MessengerCreateInfo.pUserData = nullptr;  // Optional

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
	vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	if (!vkCreateDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkCreateDebugUtilsMessenger");
	}

	VkResult res = vkCreateDebugUtilsMessenger(m_instance, &MessengerCreateInfo, NULL, &m_debugMessenger);
	LE_CHECK_VK_RESULT(res, "debug utils messenger");

	LE_DEBUG_MESSAGE("Debug utils messenger created");
}

void LeVulkanCore::CreateSurface(GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pWindow);

	VkResult Result = glfwCreateWindowSurface(m_instance, pWindow, NULL, &m_surface);
	LE_CHECK_VK_RESULT(Result, "create window surface");

	LE_DEBUG_MESSAGE("Window surface created");

}

void LeVulkanCore::CreateDevice()
{
	float qPriorities[] = { 1.0f };

	VkDeviceQueueCreateInfo qInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0, // must be zero
		.queueFamilyIndex = m_queueFamily,
		.queueCount = 1,
		.pQueuePriorities = &qPriorities[0]
	};

	const char* DevExts[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	if (m_physicalDevices.Selected().m_features.geometryShader == VK_FALSE)
		LE_ERROR("The Geometry Shader is not supported!");

	if (m_physicalDevices.Selected().m_features.tessellationShader == VK_FALSE)
		LE_ERROR("The Tessellation Shader is not supported!");


	VkPhysicalDeviceFeatures DeviceFeatures = { 0 };
	DeviceFeatures.geometryShader = VK_TRUE;
	DeviceFeatures.tessellationShader = VK_TRUE;

	VkDeviceCreateInfo DeviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &qInfo,
		.enabledLayerCount = 0,			// DEPRECATED
		.ppEnabledLayerNames = NULL,    // DEPRECATED
		.enabledExtensionCount = ARRAY_SIZE(DevExts),
		.ppEnabledExtensionNames = DevExts,
		.pEnabledFeatures = &DeviceFeatures
	};

	VkResult res = vkCreateDevice(m_physicalDevices.Selected().m_physDevice, &DeviceCreateInfo, NULL, &m_device);
	LE_CHECK_VK_RESULT(res, "Create logical device");

	LE_DEBUG_MESSAGE("Logical device created");
	
}


static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes)
{
	for (int i = 0; i < PresentModes.size(); i++) {
		if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			return PresentModes[i];
		}
	}

	// Fallback to FIFO which is always supported
	return VK_PRESENT_MODE_FIFO_KHR;
}


static uint32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	uint32 RequestedNumImages = Capabilities.minImageCount + 1;

	int FinalNumImages = 0;

	if ((Capabilities.maxImageCount > 0) && (RequestedNumImages > Capabilities.maxImageCount)) {
		FinalNumImages = Capabilities.maxImageCount;
	}
	else {
		FinalNumImages = RequestedNumImages;
	}

	return FinalNumImages;
}


static VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats)
{
	for (int i = 0; i < SurfaceFormats.size(); i++) {
		if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			return SurfaceFormats[i];
		}
	}

	return SurfaceFormats[0];
}

VkImageView CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels)
{
	VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = Image,
		.viewType = ViewType,
		.format = Format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = LayerCount
		}
	};

	VkImageView ImageView;
	VkResult res = vkCreateImageView(Device, &ViewInfo, NULL, &ImageView);
	LE_CHECK_VK_RESULT(res, "vkCreateImageView");
	return ImageView;
}


void LeVulkanCore::CreateSwapChain()
{
	const VkSurfaceCapabilitiesKHR& SurfaceCaps = m_physicalDevices.Selected().m_surfaceCaps;

	uint32 NumImages = ChooseNumImages(SurfaceCaps);

	const std::vector<VkPresentModeKHR>& PresentModes = m_physicalDevices.Selected().m_presentModes;
	VkPresentModeKHR PresentMode = ChoosePresentMode(PresentModes);

	VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormatAndColorSpace(
		m_physicalDevices.Selected().m_surfaceFormats);

	VkSwapchainCreateInfoKHR SwapChainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = m_surface,
		.minImageCount = NumImages,
		.imageFormat = SurfaceFormat.format,
		.imageColorSpace = SurfaceFormat.colorSpace,
		.imageExtent = SurfaceCaps.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &m_queueFamily,
		.preTransform = SurfaceCaps.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = PresentMode,
		.clipped = VK_TRUE
	};

	VkResult res = vkCreateSwapchainKHR(m_device, &SwapChainCreateInfo, NULL, &m_swapChain);
	LE_CHECK_VK_RESULT(res, "vkCreateSwapchainKHR");

	LE_DEBUG_MESSAGE("Swap chain created");

	uint32 NumSwapChainImages = 0;
	res = vkGetSwapchainImagesKHR(m_device, m_swapChain, &NumSwapChainImages, NULL);
	LE_CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR");
	LE_DEBUG_ASSERT(NumImages == NumSwapChainImages);

	LE_DEBUG_MESSAGE("Number of images %d", NumSwapChainImages);

	m_images.resize(NumSwapChainImages);
	m_imageViews.resize(NumSwapChainImages);

	res = vkGetSwapchainImagesKHR(m_device, m_swapChain, &NumSwapChainImages, m_images.data());
	LE_CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR");


	int LayerCount = 1;
	int MipLevels = 1;
	for (uint32 i = 0; i < NumSwapChainImages; i++) 
	{
		m_imageViews[i] = CreateImageView(m_device, m_images[i], SurfaceFormat.format,
			VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, LayerCount, MipLevels);
	}

	LE_DEBUG_MESSAGE("Created image views");
}

void LeVulkanCore::CreateCommandBufferPool()
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = m_queueFamily
	};

	VkResult res = vkCreateCommandPool(m_device, &cmdPoolCreateInfo, NULL, &m_cmdBufPool);
	LE_CHECK_VK_RESULT(res, "vkCreateCommandPool");

	LE_DEBUG_MESSAGE("Command buffer pool created");
}

void LeVulkanCore::CreateCommandBuffers(uint32 count, VkCommandBuffer* cmdBufs)
{
	VkCommandBufferAllocateInfo cmdBufAllocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = m_cmdBufPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = count
	};

	VkResult res = vkAllocateCommandBuffers(m_device, &cmdBufAllocInfo, cmdBufs);
	LE_CHECK_VK_RESULT(res, "vkAllocateCommandBuffers");

	LE_DEBUG_MESSAGE("%d command buffers created", count);
}

void LeVulkanCore::FreeCommandBuffers(uint32 Count, const VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(m_device, m_cmdBufPool, Count, pCmdBufs);
}

void LeVulkanCore::BeginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags)
{
	VkCommandBufferBeginInfo BeginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = UsageFlags,
		.pInheritanceInfo = NULL
	};

	VkResult res = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
	LE_CHECK_VK_RESULT(res, "vkBeginCommandBuffer");
}

void LeVulkanCore::EndCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res, "vkEndCommandBuffer");
}
