
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVulkanQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeMesh/LeMeshVulkan/LeMeshVulkan.h"
#include "LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"

// Lukas Andrijauskas
// 10th March, 2026

// Vulkan renderer API implementation loosely based on OGLDEV Vulkan tutorial

static GLFWwindow* s_pWindow = NULL;

static VkDevice s_LogicalDevice = VK_NULL_HANDLE;
static VkInstance s_Instance = VK_NULL_HANDLE;
static VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;
static VkSurfaceKHR s_Surface = VK_NULL_HANDLE;
static LeVkPhysicalDevices s_PhysicalDevices;
static uint32 s_QueueFamily = 0;


static LeSwapchain s_Swapchain;

static VkCommandPool s_CommandBufferPool;
static VkCommandPool s_TransientCommandBufferPool; // Command pool for one shot commands
static LeVulkanQueue s_Queue;



static void createCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = s_CommandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(s_LogicalDevice, &CommandBufferAllocateInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);

}

static void freeCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(s_LogicalDevice, s_CommandBufferPool, Count, pCmdBufs);
}


static void beginCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	VkResult res = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	LE_CHECK_VK_RESULT(res, "vkBeginCommandBuffer");
}

static void endCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res, "vkEndCommandBuffer");
}

static void createOneShotCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{

	VkCommandBufferAllocateInfo cmdBufAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = s_TransientCommandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(s_LogicalDevice, &cmdBufAllocInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);
}

static void beginOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkCommandBufferBeginInfo BeginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = NULL
	};

	VkResult res = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
	LE_CHECK_VK_RESULT(res);
}


static void endOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res, "vkEndCommandBuffer");


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
	uint32 MemoryTypeIndex = LeVkGetMemoryTypeIndex(&s_PhysicalDevices.Selected()->m_memProps, MemReqs.memoryTypeBits, Properties);

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


static void createUniformBuffer(size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{

	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	createBuffer(Size, Usage, MemProps, OutMemoryBuffer);

}

static void createUniformBuffers(size_t Size, std::vector<LeVkMemoryBuffer>& OutMemoryBuffers)
{

	OutMemoryBuffers.clear();
	OutMemoryBuffers.resize(s_Swapchain.GetNumImages());

	for (int i = 0; i < OutMemoryBuffers.size(); i++)
		createUniformBuffer(Size, &OutMemoryBuffers[i]);

}

struct FrameUniformBufferObject
{
	glm::mat4 MVP;
};


struct CurrentRenderFrame
{

	void Init()
	{
		createCommandBuffers(1, &CommandBuffer);

		ImageIndex = -1;

		FrameUBO.MVP = glm::mat4(1.0f);
		createUniformBuffer(1, &FrameUBOMemoryBuffer);

		bFramebufferResized = false;
	}
	void DeInit()
	{
		freeCommandBuffers(1, &CommandBuffer);
		FrameUBOMemoryBuffer.DeAlloc();
	}

	VkCommandBuffer CommandBuffer;

	int32 ImageIndex;

	FrameUniformBufferObject FrameUBO;
	LeVkMemoryBuffer FrameUBOMemoryBuffer;

	LeBool bFramebufferResized;
};
static CurrentRenderFrame s_CurrentRenderFrame[LE_MAX_FRAMES_IN_FLIGHT];
static int32 s_FrameIndex;
//static LeVkDescriptor


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

	VkResult res = vkCreateDebugUtilsMessenger(s_Instance, &MessengerCreateInfo, NULL, &s_DebugMessenger);
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
		.queueFamilyIndex = s_QueueFamily,
		.queueCount = 1,
		.pQueuePriorities = &qPriorities[0]
	};

	const char* DevExts[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
		VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
	};

	if (s_PhysicalDevices.Selected()->m_features.geometryShader == VK_FALSE)
		LE_ERROR("The Geometry Shader is not supported!");

	if (s_PhysicalDevices.Selected()->m_features.tessellationShader == VK_FALSE)
		LE_ERROR("The Tessellation Shader is not supported!");

	if (s_PhysicalDevices.Selected()->m_features.samplerAnisotropy == VK_FALSE)
		LE_ERROR("Sampler Anisotropy is not supported!");


	VkPhysicalDeviceVulkan13Features vk13 =
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE,
	};


	VkPhysicalDeviceFeatures2 features2 =
	{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &vk13
	};

	features2.features.geometryShader = VK_TRUE;
	features2.features.tessellationShader = VK_TRUE;
	features2.features.samplerAnisotropy = VK_TRUE;

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

	VkResult res = vkCreateDevice(s_PhysicalDevices.Selected()->m_physDevice, &DeviceCreateInfo, NULL, &s_LogicalDevice);
	LE_CHECK_VK_RESULT(res, "Create logical device");

	LE_DEBUG_MESSAGE("Logical device created");

}

static VkCommandPool createCommandBufferPool(VkCommandPoolCreateFlags Flags, uint32 QueueFamilyIndex)
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





static void copyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size)
{

	VkCommandBuffer CopyCommandBuffer;
	createOneShotCommandBuffers(1, &CopyCommandBuffer);
	beginOneShotCommandBuffer(CopyCommandBuffer);


	VkBufferCopy BufferCopy =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Size
	};

	vkCmdCopyBuffer(CopyCommandBuffer, Src, Dst, 1, &BufferCopy);

	endOneShotCommandBuffer(CopyCommandBuffer);

	// Need to wait because we dealloc buffer memory after this function call >:(
	// FIXME
	s_Queue.SubmitSync(CopyCommandBuffer);
	s_Queue.WaitIdle();

}

static void copyBufferToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkBuffer Src, uint32 width, uint32 height)
{
	VkBufferImageCopy Region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = 
		{
			VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1
		},
		.imageOffset = {0, 0, 0},
		.imageExtent = {width, height, 1}
	};

	vkCmdCopyBufferToImage(CommandBuffer, Src, Dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);
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


static void setupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(pIndices);
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
	LeMemcpy(pMem, pIndices, Size);

	// Step 4: unmap/release the mapped memory
	vkUnmapMemory(s_LogicalDevice, StagingVB.mem);

	if (OutMemoryBuffer->Alloced() && Size > OutMemoryBuffer->allocationSize)
		OutMemoryBuffer->DeAlloc();

	if (!OutMemoryBuffer->Alloced())
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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


	s_Queue.WaitIdle();



	s_Swapchain.Destroy();
	s_Swapchain.Init(s_LogicalDevice, s_PhysicalDevices.Selected(), s_Surface, s_QueueFamily);
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
		.NumImages = (int32)s_Swapchain.GetNumImages(),
		.RenderColFormat = s_Swapchain.GetImageFormat(),
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

			LeVkGraphicsPipeline* NewGraphicsPipeline = new LeVkGraphicsPipeline(PipelineKeyHash, s_Swapchain.GetNumImages(),
				pShaderVk, s_Swapchain.GetImageFormat());

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

// returns true if buffer was created or updated
static LeBool setupIndexBufferForMatBlock(LeMatBlockVk* pMbVk)
{
	LE_DEBUG_ASSERT(pMbVk);

	if (pMbVk->HasFlags(MATBLOCK_FACES_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMbVk->Faces.size() * sizeof(LeVertex);
		setupIndexBuffer(pMbVk->Faces.data(), uLocalSizeOfVerts, pMbVk->GetIndexBuffer()); // this will either create or update the buffer
		pMbVk->DeleteFlags(MATBLOCK_FACES_DIRTY);

		return true;
	}


	return false;

}



static void updateVkImage(VkCommandBuffer CommandBuffer, LeTextureVk* pTex)
{

	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pTex);

	int BytesPerPixel = GetVkBytesPerTexFormat(pTex->GetVkFormat());

	VkDeviceSize LayerSize = pTex->GetWidth() * pTex->GetHeight() * BytesPerPixel;
	int LayerCount = 1;
	VkDeviceSize ImageSize = LayerCount * LayerSize;

	LeVkTransitionImageLayout(CommandBuffer, pTex->textureImage.image,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT);

	copyBufferToImage(CommandBuffer, pTex->textureImage.image, pTex->stagingBuffer.buffer, pTex->GetWidth(), pTex->GetHeight());

	LeVkTransitionImageLayout(CommandBuffer, pTex->textureImage.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
}



void LeInitVulkan(const char* pAppName, GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pAppName);
	LE_DEBUG_ASSERT(pWindow);

	s_pWindow = pWindow;

	createInstance(pAppName);
	createDebugCallback();
	createSurface(pWindow);
	s_PhysicalDevices.Init(s_Instance, s_Surface);
	s_QueueFamily = s_PhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
	createLogicalDevice();
	s_Swapchain.Init(s_LogicalDevice, s_PhysicalDevices.Selected(), s_Surface, s_QueueFamily);

	LeVkInitDescriptorSets(s_LogicalDevice);

	s_CommandBufferPool = createCommandBufferPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, s_QueueFamily);
	s_TransientCommandBufferPool = createCommandBufferPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, s_QueueFamily);

	for (int32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
	{
		s_CurrentRenderFrame[i].Init();
		LeVkLinkDescriptorSets(&s_CurrentRenderFrame[i].FrameUBOMemoryBuffer, i);
	}

	s_FrameIndex = 0;
	
	s_Queue.Init(s_LogicalDevice, &s_Swapchain, s_QueueFamily, 0);


}

void LeShutDownVulkan(void)
{
	// currently just this.. hack until I sort my world objects
	s_Queue.WaitIdle();
}

void LeDeInitVulkan(void)
{

	// Destroy graphics pipelines
	LeVkGraphicsPipelineDictionary::Singleton()->Purge();

	// Destroy the render queue
	s_Queue.Destroy();

	for (int32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
		s_CurrentRenderFrame[i].DeInit();

	// Destroy command pool
	vkDestroyCommandPool(s_LogicalDevice, s_TransientCommandBufferPool, NULL);
	vkDestroyCommandPool(s_LogicalDevice, s_CommandBufferPool, NULL);

	// Destroy descriptor sets

	LeVkDeInitDescriptorSets();

	// Destroy swapchain
	s_Swapchain.Destroy();

	// Destroy logical device

	vkDestroyDevice(s_LogicalDevice, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");

	// Clear physical devices

	s_PhysicalDevices.DeInit();

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

	vkDestroyDebugUtilsMessenger(s_Instance, s_DebugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");

	// Destroy instance

	vkDestroyInstance(s_Instance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
}


LeBool LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	LE_DEBUG_ASSERT(pCam);

	s_Queue.WaitForFences(s_FrameIndex);

	CurrentRenderFrame* pRenderFrame = &s_CurrentRenderFrame[s_FrameIndex];
	pRenderFrame->bFramebufferResized = bFramebufferResized;

	pRenderFrame->ImageIndex = s_Queue.AcquireNextImage(s_FrameIndex);
	if (pRenderFrame->ImageIndex == -1)
	{
		recreateSwapChain();
		return false;
	}

	s_Queue.ResetFences(s_FrameIndex);

	// Update frame UBOs

	pRenderFrame->FrameUBO.MVP = *pCam->GetViewProjMatrix();

	s_CurrentRenderFrame[s_FrameIndex].FrameUBOMemoryBuffer.Update(
		&s_CurrentRenderFrame[s_FrameIndex].FrameUBO, sizeof(s_CurrentRenderFrame[s_FrameIndex].FrameUBO));

	// Begin recording the command buffer

	VkCommandBuffer pCommandBuffer = s_CurrentRenderFrame[s_FrameIndex].CommandBuffer;
	beginCommandBuffer(pCommandBuffer);

	// setup image barrier for clearing the color

	LeVkTransitionImageLayout(pCommandBuffer, s_Swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);


	VkExtent2D swapchainExtent = s_Swapchain.GetExtent();

	VkClearValue ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = s_Swapchain.GetImageView(pRenderFrame->ImageIndex);
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue = ClearColor;

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = swapchainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;

	vkCmdBeginRendering(pCommandBuffer, &renderingInfo);

	VkViewport Viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)swapchainExtent.width,
		.height = (float)swapchainExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D Scissor =
	{
		.offset =
		{
			.x = 0,
			.y = 0,
		},
		.extent =
		{
			.width = (uint32)swapchainExtent.width,
			.height = (uint32)swapchainExtent.height
		}
	};


	vkCmdSetViewport(pCommandBuffer, 0, 1, &Viewport);
	vkCmdSetScissor(pCommandBuffer, 0, 1, &Scissor);


	return true;
}

LeBool LeVkRender(LeMesh* pMesh)
{
	LE_DEBUG_ASSERT(pMesh);

	CurrentRenderFrame* pRenderFrame = &s_CurrentRenderFrame[s_FrameIndex];
	VkCommandBuffer pCommandBuffer = s_CurrentRenderFrame[s_FrameIndex].CommandBuffer;

	for (uint32 i = 0; i < pMesh->GetNumMatblocks(); i++)
	{
		LeMatBlock* pMb = pMesh->GetMatblock(i);
		LE_DEBUG_ASSERT(pMb);

		LeMatBlockVk* pMbVk = static_cast<LeMatBlockVk*>(pMb);
		LE_DEBUG_ASSERT(pMbVk);

		refreshGraphicsPipelineForMatblock(pMbVk);


		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMbVk->GetGraphicsPipeline()->GetPipeline());

		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMbVk->GetGraphicsPipeline()->GetPipelineLayout(),
			0,  // firstSet
			1,  // descriptorSetCount
			LeVkGetDescriptorSet(s_FrameIndex),
			0,	// dynamicOffsetCount
			NULL);	// pDynamicOffsets

		setupVertexBufferForMatBlock(pMbVk);
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &pMbVk->GetVertexBuffer()->buffer, offsets);

		setupIndexBufferForMatBlock(pMbVk);
		vkCmdBindIndexBuffer(pCommandBuffer, pMbVk->GetIndexBuffer()->buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(pCommandBuffer, uint32(pMbVk->Faces.size()) * 3, 1, 0, 0, 0);

	}

	return true;
}

LeBool LeVkEndRender(void)
{

	CurrentRenderFrame* pRenderFrame = &s_CurrentRenderFrame[s_FrameIndex];
	VkCommandBuffer pCommandBuffer = s_CurrentRenderFrame[s_FrameIndex].CommandBuffer;

	vkCmdEndRendering(pCommandBuffer);


	// transition the image for presenting

	LeVkTransitionImageLayout(pCommandBuffer, s_Swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

	endCommandBuffer(pCommandBuffer);

	s_Queue.SubmitAsync(pCommandBuffer, pRenderFrame->ImageIndex, s_FrameIndex);


	if (!s_Queue.Present(pRenderFrame->ImageIndex, s_FrameIndex) || pRenderFrame->bFramebufferResized)
		recreateSwapChain();

	s_FrameIndex++;
	s_FrameIndex = s_FrameIndex % LE_MAX_FRAMES_IN_FLIGHT;

	return true;
}

// global functions

VkDevice LeVkGetLogicalDevice(void)
{
	LE_DEBUG_ASSERT(s_LogicalDevice);
	return s_LogicalDevice;
}

const LeVkPhysicalDevice* LeVkGetPhysicalDevice(void)
{
	const LeVkPhysicalDevice* pPhysicalDevice = s_PhysicalDevices.Selected();
	LE_DEBUG_ASSERT(pPhysicalDevice);

	return pPhysicalDevice;
}