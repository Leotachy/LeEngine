
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeRenderUtils.h"


static const VkVertexInputBindingDescription s_BindingDescription =
{
	0, sizeof(LeVertex), VK_VERTEX_INPUT_RATE_VERTEX
};

const VkVertexInputBindingDescription* LeVertexGetBindingDescription()
{
	return &s_BindingDescription;
}

static const VkVertexInputAttributeDescription s_AttributeDescriptions[] =
{
	VkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
	VkVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(LeVertex::pos))
};

const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions()
{
	return s_AttributeDescriptions;
}

int32 LeVertexGetNumAttributeDescriptions()
{
	return ARRAY_SIZE(s_AttributeDescriptions);
}


void LeMemoryBuffer::Update(VkDevice Device, const void* pData, size_t Size)
{
	void* pMem = NULL;
	VkResult res = vkMapMemory(Device, m_mem, 0, Size, 0, &pMem);
	LE_CHECK_VK_RESULT(res, "vkMapMemory");
	LeMemcpy(pMem, pData, Size);
	vkUnmapMemory(Device, m_mem);
}

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

	// Destroy the copy command buffer

	FreeCommandBuffers(1, &m_copyCmdBuf);

	// Destroy the render queue
	m_queue.Destroy();

	// Destroy command pool
	vkDestroyCommandPool(m_device, m_cmdBufPool, NULL);

	// Destroy swapchain
	m_swapChain.Destroy();

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
	LE_DEBUG_ASSERT(pAppName);
	LE_DEBUG_ASSERT(pWindow);

	m_pWindow = pWindow;

	CreateInstance(pAppName);
	CreateDebugCallback();
	CreateSurface(pWindow);
	m_physicalDevices.Init(m_instance, m_surface);
	m_queueFamily = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
	CreateDevice();
	m_swapChain.Init(m_device, &m_physicalDevices.Selected(), m_surface, m_queueFamily);
	CreateCommandBufferPool();
	m_queue.Init(m_device, &m_swapChain, m_queueFamily, 0);

	CreateCommandBuffers(1, &m_copyCmdBuf);
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
		.apiVersion = VK_API_VERSION_1_4
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
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
		VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
	};

	if (m_physicalDevices.Selected().m_features.geometryShader == VK_FALSE)
		LE_ERROR("The Geometry Shader is not supported!");

	if (m_physicalDevices.Selected().m_features.tessellationShader == VK_FALSE)
		LE_ERROR("The Tessellation Shader is not supported!");


	VkPhysicalDeviceVulkan13Features vk13 =
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE
	};
	

	VkPhysicalDeviceFeatures2 features2 =
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &vk13
	};

	features2.features.geometryShader = VK_TRUE;
	features2.features.tessellationShader = VK_TRUE;

	VkDeviceCreateInfo DeviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &features2,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &qInfo,
		.enabledLayerCount = 0,			// DEPRECATED
		.ppEnabledLayerNames = NULL,    // DEPRECATED
		.enabledExtensionCount = ARRAY_SIZE(DevExts),
		.ppEnabledExtensionNames = DevExts
	};

	VkResult res = vkCreateDevice(m_physicalDevices.Selected().m_physDevice, &DeviceCreateInfo, NULL, &m_device);
	LE_CHECK_VK_RESULT(res, "Create logical device");

	LE_DEBUG_MESSAGE("Logical device created");
	
}

void LeVulkanCore::CreateCommandBufferPool()
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = m_queueFamily
	};

	VkResult res = vkCreateCommandPool(m_device, &cmdPoolCreateInfo, NULL, &m_cmdBufPool);
	LE_CHECK_VK_RESULT(res, "vkCreateCommandPool");

	LE_DEBUG_MESSAGE("Command buffer pool created");
}



uint32 LeVulkanCore::GetMemoryTypeIndex(uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
{

	const VkPhysicalDeviceMemoryProperties& MemProps = m_physicalDevices.Selected().m_memProps;

	for (uint32 i = 0; i < MemProps.memoryTypeCount; i++)
	{
		const VkMemoryType& MemType = MemProps.memoryTypes[i];
		uint32 CurBitmask = (1 << i);
		bool IsCurMemTypeSupported = (MemTypeBitsMask & CurBitmask);
		bool HasRequiredMemProps = ((MemType.propertyFlags & ReqMemPropFlags) == ReqMemPropFlags);

		if (IsCurMemTypeSupported && HasRequiredMemProps)
		{
			return i;
		}
	}

	LE_DEBUG_ERROR("Cannot find memory type for type %x requested mem props %x", MemTypeBitsMask, ReqMemPropFlags);

	return -1;
}

void LeVulkanCore::CopyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size)
{
	BeginCommandBuffer(m_copyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy BufferCopy = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Size
	};

	vkCmdCopyBuffer(m_copyCmdBuf, Src, Dst, 1, &BufferCopy);

	vkEndCommandBuffer(m_copyCmdBuf);

	m_queue.SubmitSync(m_copyCmdBuf);

	m_queue.WaitIdle();
}

void LeVulkanCore::CreateUniformBuffer(size_t Size, LeMemoryBuffer& OutMemoryBuffer)
{

	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	CreateBuffer(Size, Usage, MemProps, OutMemoryBuffer);

}


void LeVulkanCore::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties,
	LeMemoryBuffer& OutMemoryBuffer)
{
	VkBufferCreateInfo BufferCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = Size,
		.usage = Usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};



	// Step 1: create a buffer
	VkResult res = vkCreateBuffer(m_device, &BufferCreateInfo, NULL, &OutMemoryBuffer.m_buffer);
	LE_CHECK_VK_RESULT(res, "vkCreateBuffer");

	// Step 2: get the buffer memory requirements
	VkMemoryRequirements MemReqs = {};
	vkGetBufferMemoryRequirements(m_device, OutMemoryBuffer.m_buffer, &MemReqs);

	OutMemoryBuffer.m_allocationSize = MemReqs.size;

	// Step 3: get the memory type index
	uint32 MemoryTypeIndex = GetMemoryTypeIndex(MemReqs.memoryTypeBits, Properties);

	// Step 4: allocate memory
	VkMemoryAllocateInfo MemAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = MemReqs.size,
		.memoryTypeIndex = MemoryTypeIndex
	};

	res = vkAllocateMemory(m_device, &MemAllocInfo, NULL, &OutMemoryBuffer.m_mem);
	LE_CHECK_VK_RESULT(res, "vkAllocateMemory");

	// Step 5: bind memory
	res = vkBindBufferMemory(m_device, OutMemoryBuffer.m_buffer, OutMemoryBuffer.m_mem, 0);
	LE_CHECK_VK_RESULT(res, "vkBindBufferMemory");
}

void LeVulkanCore::CreateUniformBuffers(size_t Size, std::vector<LeMemoryBuffer>& OutMemoryBuffers)
{

	OutMemoryBuffers.clear();
	OutMemoryBuffers.resize(m_swapChain.GetNumImages());

	for (int i = 0; i < OutMemoryBuffers.size(); i++)
	{
		CreateUniformBuffer(Size, OutMemoryBuffers[i]);
	}

}

void LeVulkanCore::CreateVertexBuffer(const void* pVertices, size_t Size, LeMemoryBuffer& OutMemoryBuffer)
{
	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeMemoryBuffer StagingVB;
	CreateBuffer(Size, Usage, MemProps, StagingVB);

	// Step 2: map the memory of the stage buffer
	void* pMem = NULL;
	VkDeviceSize Offset = 0;
	VkMemoryMapFlags Flags = 0;
	VkResult res = vkMapMemory(m_device, StagingVB.m_mem, Offset, StagingVB.m_allocationSize, Flags, &pMem);
	LE_CHECK_VK_RESULT(res, "vkMapMemory");

	// Step 3: copy the vertices to the staging buffer
	LeMemcpy(pMem, pVertices, Size);

	// Step 4: unmap/release the mapped memory
	vkUnmapMemory(m_device, StagingVB.m_mem);

	// Step 5: create the final buffer
	Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	CreateBuffer(Size, Usage, MemProps, OutMemoryBuffer);

	// Step 6: copy the staging buffer to the final buffer
	CopyBuffer(OutMemoryBuffer.m_buffer, StagingVB.m_buffer, Size);

	// Step 7: release the resources of the staging buffer
	StagingVB.Destroy(m_device);
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

void LeVulkanCore::RecreateSwapChain(void)
{

	// minimisation case.. TODO obv
	int32 width = 0, height = 0;
	glfwGetFramebufferSize(m_pWindow, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_pWindow, &width, &height);
		glfwWaitEvents();
	}


	m_queue.WaitIdle();



	m_swapChain.Destroy();
	m_swapChain.Init(m_device, &m_physicalDevices.Selected(), m_surface, m_queueFamily);
}

