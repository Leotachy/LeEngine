
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LePhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVulkanQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"

// Lukas Andrijauskas
// 10th March, 2026

// Vulkan renderer API implementation loosely based on OGLDEV Vulkan tutorial

static GLFWwindow* s_pWindow = NULL;

static VkDevice s_LogicalDevice = VK_NULL_HANDLE;
static VkInstance s_Instance = VK_NULL_HANDLE;
static VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
static VkSurfaceKHR s_Surface = VK_NULL_HANDLE;
static LeVulkanPhysicalDevices m_physicalDevices;
static uint32 m_queueFamily = 0;


static LeSwapchain m_swapChain;

static VkCommandPool m_cmdBufPool;
static VkCommandPool m_cmdBufPoolTransient; // Command pool for short lived commands
static LeVulkanQueue m_queue;


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
	VkDebugUtilsMessageTypeFlagsEXT Type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{


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

	return VK_FALSE;  // The calling function should not be aborted
}


static void createInstance(const char* pAppName)
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


	VkResult res = vkCreateInstance(&CreateInfo, NULL, &s_Instance);
	LE_CHECK_VK_RESULT(res, "Create instance");

	LE_DEBUG_MESSAGE("Vulkan instance created");

}



static void createDebugCallback()
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
	MessengerCreateInfo.pfnUserCallback = debugCallback;
	MessengerCreateInfo.pUserData = nullptr;  // Optional

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
	vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkCreateDebugUtilsMessengerEXT");
	if (!vkCreateDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkCreateDebugUtilsMessenger");
	}

	VkResult res = vkCreateDebugUtilsMessenger(s_Instance, &MessengerCreateInfo, NULL, &m_debugMessenger);
	LE_CHECK_VK_RESULT(res, "debug utils messenger");

	LE_DEBUG_MESSAGE("Debug utils messenger created");
}

static void createSurface(GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pWindow);

	VkResult Result = glfwCreateWindowSurface(s_Instance, pWindow, NULL, &s_Surface);
	LE_CHECK_VK_RESULT(Result, "create window surface");

	LE_DEBUG_MESSAGE("Window surface created");

}

static void createLogicalDevice()
{
	float qPriorities[] = { 1.0f };

	VkDeviceQueueCreateInfo qInfo =
	{
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

	VkDeviceCreateInfo DeviceCreateInfo =
	{
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

	VkResult res = vkCreateDevice(m_physicalDevices.Selected().m_physDevice, &DeviceCreateInfo, NULL, &s_LogicalDevice);
	LE_CHECK_VK_RESULT(res, "Create logical device");

	LE_DEBUG_MESSAGE("Logical device created");

}

static VkCommandPool createCommandBufferPool(VkCommandPoolCreateFlagBits Flags, uint32 QueueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = Flags,
		.queueFamilyIndex = QueueFamilyIndex
	};

	VkCommandPool ret;

	VkResult res = vkCreateCommandPool(s_LogicalDevice, &cmdPoolCreateInfo, NULL, &ret);
	LE_CHECK_VK_RESULT(res, "vkCreateCommandPool");

	LE_DEBUG_MESSAGE("Command buffer pool created");

	return ret;
}

void createCommandBuffers(VkCommandPool CommandPool, uint32 count, VkCommandBuffer* cmdBufs)
{
	VkCommandBufferAllocateInfo cmdBufAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = CommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = count
	};

	VkResult res = vkAllocateCommandBuffers(s_LogicalDevice, &cmdBufAllocInfo, cmdBufs);
	LE_CHECK_VK_RESULT(res, "vkAllocateCommandBuffers");

	LE_DEBUG_MESSAGE("%d command buffers created", count);
}

static void freeCommandBuffers(uint32 Count, const VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(s_LogicalDevice, m_cmdBufPool, Count, pCmdBufs);
}

static void beginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags)
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

static void endCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res, "vkEndCommandBuffer");
}

static uint32 getMemoryTypeIndex(uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
{

	const VkPhysicalDeviceMemoryProperties& MemProps = m_physicalDevices.Selected().m_memProps;

	for (uint32 i = 0; i < MemProps.memoryTypeCount; i++)
	{
		uint32 CurBitmask = (1 << i);
		LeBool IsCurMemTypeSupported = (MemTypeBitsMask & CurBitmask);

		if (!IsCurMemTypeSupported)
			continue;

		const VkMemoryType& MemType = MemProps.memoryTypes[i];
		LeBool HasRequiredMemProps = ((MemType.propertyFlags & ReqMemPropFlags) == ReqMemPropFlags);

		if (!HasRequiredMemProps)
			continue;

		return i;
	}

	LE_DEBUG_ERROR("Cannot find memory type for type %x requested mem props %x", MemTypeBitsMask, ReqMemPropFlags);

	return -1;
}

static void copyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size)
{

	VkCommandBuffer copyCmdBuf;
	createCommandBuffers(m_cmdBufPoolTransient, 1, &copyCmdBuf);

	beginCommandBuffer(copyCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy BufferCopy =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Size
	};

	vkCmdCopyBuffer(copyCmdBuf, Src, Dst, 1, &BufferCopy);

	vkEndCommandBuffer(copyCmdBuf);

	m_queue.SubmitSync(copyCmdBuf);

	m_queue.WaitIdle();
}

static void createUniformBuffer(size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{

	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	createBuffer(Size, Usage, MemProps, OutMemoryBuffer);

}


static void createBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties,
	LeVkMemoryBuffer* OutMemoryBuffer)
{

	LE_DEBUG_ASSERT(OutMemoryBuffer);

	VkBufferCreateInfo BufferCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = Size,
		.usage = Usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};


	OutMemoryBuffer->device = s_LogicalDevice;

	// Step 1: create a buffer
	VkResult res = vkCreateBuffer(s_LogicalDevice, &BufferCreateInfo, NULL, &OutMemoryBuffer->buffer);
	LE_CHECK_VK_RESULT(res, "vkCreateBuffer");

	// Step 2: get the buffer memory requirements
	VkMemoryRequirements MemReqs = {};
	vkGetBufferMemoryRequirements(s_LogicalDevice, OutMemoryBuffer->buffer, &MemReqs);

	OutMemoryBuffer->allocationSize = MemReqs.size;

	// Step 3: get the memory type index
	uint32 MemoryTypeIndex = getMemoryTypeIndex(MemReqs.memoryTypeBits, Properties);

	// Step 4: allocate memory
	VkMemoryAllocateInfo MemAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = MemReqs.size,
		.memoryTypeIndex = MemoryTypeIndex
	};

	res = vkAllocateMemory(s_LogicalDevice, &MemAllocInfo, NULL, &OutMemoryBuffer->mem);
	LE_CHECK_VK_RESULT(res, "vkAllocateMemory");

	// Step 5: bind memory
	res = vkBindBufferMemory(s_LogicalDevice, OutMemoryBuffer->buffer, OutMemoryBuffer->mem, 0);
	LE_CHECK_VK_RESULT(res, "vkBindBufferMemory");
}

static void createUniformBuffers(size_t Size, std::vector<LeVkMemoryBuffer>& OutMemoryBuffers)
{

	OutMemoryBuffers.clear();
	OutMemoryBuffers.resize(m_swapChain.GetNumImages());

	for (int i = 0; i < OutMemoryBuffers.size(); i++)
		createUniformBuffer(Size, &OutMemoryBuffers[i]);

}

static void setupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(pVertices);
	LE_DEBUG_ASSERT(Size);
	LE_DEBUG_ASSERT(OutMemoryBuffer);

	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeVkMemoryBuffer StagingVB;
	createBuffer(Size, Usage, MemProps, &StagingVB);

	// Step 2: map the memory of the stage buffer
	void* pMem = NULL;
	VkDeviceSize Offset = 0;
	VkMemoryMapFlags Flags = 0;
	VkResult res = vkMapMemory(s_LogicalDevice, StagingVB.mem, Offset, StagingVB.allocationSize, Flags, &pMem);
	LE_CHECK_VK_RESULT(res, "vkMapMemory");

	// Step 3: copy the vertices to the staging buffer
	LeMemcpy(pMem, pVertices, Size);

	// Step 4: unmap/release the mapped memory
	vkUnmapMemory(s_LogicalDevice, StagingVB.mem);

	if (OutMemoryBuffer->Alloced() && Size > OutMemoryBuffer->allocationSize)
		OutMemoryBuffer->DeAlloc();

	if (!OutMemoryBuffer->Alloced())
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		createBuffer(Size, Usage, MemProps, OutMemoryBuffer);
	}
	

	// Step 6: copy the staging buffer to the final buffer
	copyBuffer(OutMemoryBuffer->buffer, StagingVB.buffer, Size);

	// Step 7: release the resources of the staging buffer
	//StagingVB.Destroy(s_LogicalDevice); // Destructor takes care of it now
}


static void recreateSwapChain(void)
{

	// minimisation case.. TODO obv
	int32 width = 0, height = 0;
	glfwGetFramebufferSize(s_pWindow, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(s_pWindow, &width, &height);
		glfwWaitEvents();
	}


	m_queue.WaitIdle();



	m_swapChain.Destroy();
	m_swapChain.Init(s_LogicalDevice, &m_physicalDevices.Selected(), s_Surface, m_queueFamily);
}



void LeInitVulkan(const char* pAppName, GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pAppName);
	LE_DEBUG_ASSERT(pWindow);

	s_pWindow = pWindow;

	createInstance(pAppName);
	createDebugCallback();
	createSurface(pWindow);
	m_physicalDevices.Init(s_Instance, s_Surface);
	m_queueFamily = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
	createLogicalDevice();
	m_swapChain.Init(s_LogicalDevice, &m_physicalDevices.Selected(), s_Surface, m_queueFamily);
	m_cmdBufPool = createCommandBufferPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_queueFamily);
	m_cmdBufPoolTransient = createCommandBufferPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queueFamily);
	m_queue.Init(s_LogicalDevice, &m_swapChain, m_queueFamily, 0);
}

void LeTerminateVulkan(void)
{

	// Destroy the render queue
	m_queue.Destroy();

	// Destroy command pool
	vkDestroyCommandPool(s_LogicalDevice, m_cmdBufPoolTransient, NULL);
	vkDestroyCommandPool(s_LogicalDevice, m_cmdBufPool, NULL);

	// Destroy swapchain
	m_swapChain.Destroy();

	// Destroy logical device

	vkDestroyDevice(s_LogicalDevice, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");

	// Clear physical devices

	m_physicalDevices.DeInit();

	// Destroy surface

	PFN_vkDestroySurfaceKHR vkDestroySurface = VK_NULL_HANDLE;
	vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(s_Instance, "vkDestroySurfaceKHR");
	if (!vkDestroySurface)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroySurfaceKHR(s_Instance, s_Surface, NULL);
	LE_DEBUG_MESSAGE("Window surface destroyed");

	// Destroy debug utils messenger

	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
	vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!vkDestroyDebugUtilsMessenger)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroyDebugUtilsMessenger(s_Instance, m_debugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");

	// Destroy instance

	vkDestroyInstance(s_Instance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
}

LeBool LeVkBeginRender(LeCamera* pCam)
{

}

// returns true if graphics pipeline changed
// TODO: optimize this
static LeBool refreshGraphicsPipelineForMatblock(LeMatBlockVk* pMbVk)
{

	LE_DEBUG_ASSERT(pMbVk);

	// Test if graphics pipeline needs a refresh

	LeVkGraphicsPipeline* pGraphicsPipeline = pMbVk->GetGraphicsPipeline();

	
	LeMaterial* pMaterial = pMbVk->GetMaterial();
	LE_DEBUG_ASSERT(pMaterial && "Matblock is missing material!");

	LeShader* pShader = pMaterial->GetShader();
	LE_DEBUG_ASSERT(pShader && "Shader is missing material!");

	// Test if pipeline is still valid
	LeGraphicsPipelineVkKey PipelineKey =
	{
		.NumImages = m_swapChain.GetNumImages(),
		.RenderColFormat = m_swapChain.GetImageFormat(),
		.ShaderHash = pShader->GetNameHash(),
	};

	int64 PipelineKeyHash = lehash64_bin(&PipelineKey, sizeof(PipelineKey));

	if (!pGraphicsPipeline || pGraphicsPipeline->GetKey() != PipelineKeyHash)
	{

		LeVkGraphicsPipeline* FoundGraphicsPipeline = 
			LeVkGraphicsPipelineDictionary::Singleton()->Find(PipelineKeyHash);

		if (FoundGraphicsPipeline)
		{
			pMbVk->SetGraphicsPipeline(FoundGraphicsPipeline);
				
		}
		else
		{
			LeShaderVk* pShaderVk = static_cast<LeShaderVk*>(pShader);
			LE_DEBUG_ASSERT(pShaderVk);

			LeVkGraphicsPipeline* NewGraphicsPipeline = new LeVkGraphicsPipeline(PipelineKeyHash, m_swapChain.GetNumImages(),
				pShaderVk, m_swapChain.GetImageFormat());

			LeVkGraphicsPipelineDictionary::Singleton()->Add(NewGraphicsPipeline);

			pMbVk->SetGraphicsPipeline(NewGraphicsPipeline);
		}


		return true;
	}
	

	return false;
}

// returns true if buffer was created or updated
static LeBool setupVertexBufferForMatBlock(LeMatBlockVk* pMbVk)
{
	LE_DEBUG_ASSERT(pMbVk);

	if (pMbVk->HasFlags(MATBLOCK_VERTS_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMbVk->Vertices.size() * sizeof(LeVertex);
		setupVertexBuffer(pMbVk->Vertices.data(), uLocalSizeOfVerts, pMbVk->GetVertexBuffer()); // this will either create or update the buffer
		pMbVk->DeleteFlags(MATBLOCK_VERTS_DIRTY);

		return true;
	}


	return false;
	
}

void LeVkRender(LeMesh* pMesh)
{
	LE_DEBUG_ASSERT(pMesh);

	for (uint32 i = 0; i < pMesh->GetNumMatblocks(); i++)
	{
		LeMatBlock* pMb = pMesh->GetMatblock(i);
		LE_DEBUG_ASSERT(pMb);

		LeMatBlockVk* pMbVk = static_cast<LeMatBlockVk*>(pMb);
		LE_DEBUG_ASSERT(pMbVk);

		refreshGraphicsPipelineForMatblock(pMbVk);

		setupVertexBufferForMatBlock(pMbVk);



	}
}

void LeVkEndRender(void)
{

}

// global functions

VkDevice LeGetVkLogicalDevice(void)
{
	LE_DEBUG_ASSERT(s_LogicalDevice);
	return s_LogicalDevice;
}


const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity)
{
	switch (Severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";

	default:
		LE_ERROR("Invalid severity code %d\n", Severity);
	}

	return "NO SUCH SEVERITY!";
}


const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type)
{
	switch (Type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
		return "Device address binding";

	default:
		LE_ERROR("Invalid type code %d\n", Type);
		exit(1);
	}

	return "NO SUCH TYPE!";
}


uint32_t GetVkBytesPerTexFormat(VkFormat Format)
{
	switch (Format)
	{
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UNORM:
		return 1;
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R16G16_SFLOAT:
		return 4;
	case VK_FORMAT_R16G16_SNORM:
		return 4;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 4 * sizeof(uint16);
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4 * sizeof(float32);
	default:
		break;
	}

	return 0;
}



