
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL leVkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
	VkDebugUtilsMessageTypeFlagsEXT Type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{


	LeChar Buf[4096];

	sprintf(Buf,
		"Debug callback: %s\n\t Severity %s\n\t Type %s\n\t Objects:",
		pCallbackData->pMessage, LeVkUtils::GetVkDebugUtilsMessageSeverityStr(Severity), LeVkUtils::GetVkDebugUtilsMessageType(Type));

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


LeVkCore& LeVkCore::Singleton()
{
	static LeVkCore LeVulkanCore;
	return LeVulkanCore;
}

void LeVkCore::Init(const char* pAppName, GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pAppName);
	LE_DEBUG_ASSERT(pWindow);

	m_gltfWindow = pWindow;

	CreateInstance(pAppName);
	CreateDebugCallback();
	CreateSurface(pWindow);
	m_physicalDevices.Init(m_instance, m_surface);
	m_queueFamily = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
	CreateLogicalDevice();
	m_queueFamily = 0;
	m_swapchain.Init(m_logicalDevice, m_physicalDevices.Selected(), m_surface, m_queueFamily);

	LeVkDescriptorPoolManager::Singleton().Init();

	m_commandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_queueFamily);
	m_transientCommandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queueFamily);

	for (int32 i = 0; i < GetMaxFramesInFlight(); i++)
	{
		LeVkCurrentRenderFrame& CurrentRenderFrame = m_CurrentRenderFrame[i];
		CreateCommandBuffers(1, &CurrentRenderFrame.CommandBuffer);
		CurrentRenderFrame.ImageIndex = -1;
		CurrentRenderFrame.FrameUBO.MVP = glm::mat4(1.0f);
		LeVkUtils::CreateUniformBuffer(m_logicalDevice, &m_physicalDevices.Selected()->m_memProps, 
			sizeof(LeVkFrameUniformData), &CurrentRenderFrame.FrameUBOMemoryBuffer);
		CurrentRenderFrame.bFramebufferResized = false;
		CurrentRenderFrame.DescriptorSet.Init();
		CurrentRenderFrame.DescriptorSet.UpdateSet(&CurrentRenderFrame.FrameUBOMemoryBuffer);
	}

	m_FrameIndex = 0;

	m_queue.Init(m_logicalDevice, &m_swapchain, m_queueFamily, 0);
}

void LeVkCore::ShutDown(void)
{
	// currently just this.. hack until I sort my world objects
	m_queue.WaitIdle();
}

void LeVkCore::DeInit(void)
{
	// DeInit graphics pipelines
	LeVkGraphicsPipelineDictionary::Singleton()->Purge();

	// DeInit the render queue
	m_queue.Destroy();

	for (int32 i = 0; i < GetMaxFramesInFlight(); i++)
	{
		LeVkCurrentRenderFrame& CurrentRenderFrame = m_CurrentRenderFrame[i];
		FreeCommandBuffers(1, &CurrentRenderFrame.CommandBuffer);
		LeVkUtils::DestroyBuffer(&CurrentRenderFrame.FrameUBOMemoryBuffer);
	}



	// DeInit command pool
	vkDestroyCommandPool(m_logicalDevice, m_transientCommandBufferPool, NULL);
	vkDestroyCommandPool(m_logicalDevice, m_commandBufferPool, NULL);

	// DeInit descriptor pools

	LeVkDescriptorPoolManager::Singleton().DeInit();

	// DeInit swapchain
	m_swapchain.DeInit();

	// DeInit logical device

	vkDestroyDevice(m_logicalDevice, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");

	// Clear physical devices

	m_physicalDevices.DeInit();

	// DeInit surface

	PFN_vkDestroySurfaceKHR vkDestroySurface = VK_NULL_HANDLE;
	vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
	if (!vkDestroySurface)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroySurfaceKHR(m_instance, m_surface, NULL);
	LE_DEBUG_MESSAGE("Window surface destroyed");

	// DeInit debug utils messenger

	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
	vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!vkDestroyDebugUtilsMessenger)
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");

	vkDestroyDebugUtilsMessenger(m_instance, m_debugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");

	// DeInit instance

	vkDestroyInstance(m_instance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
}

LeBool LeVkCore::LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	LE_DEBUG_ASSERT(pCam);

	m_queue.WaitForFences(m_FrameIndex);

	LeVkCurrentRenderFrame* pRenderFrame = &m_CurrentRenderFrame[m_FrameIndex];
	pRenderFrame->bFramebufferResized = bFramebufferResized;

	pRenderFrame->ImageIndex = m_queue.AcquireNextImage(m_FrameIndex);
	if (pRenderFrame->ImageIndex == -1)
	{
		RecreateSwapChain();
		return false;
	}

	m_queue.ResetFences(m_FrameIndex);

	// Update frame UBOs

	pRenderFrame->FrameUBO.MVP = *pCam->GetViewProjMatrix();

	LeVkUtils::UpdateBuffer(&m_CurrentRenderFrame[m_FrameIndex].FrameUBOMemoryBuffer,
		&m_CurrentRenderFrame[m_FrameIndex].FrameUBO,
		sizeof(m_CurrentRenderFrame[m_FrameIndex].FrameUBO));

	// Begin recording the command buffer

	VkCommandBuffer pCommandBuffer = m_CurrentRenderFrame[m_FrameIndex].CommandBuffer;
	BeginCommandBuffer(pCommandBuffer);

	// setup image barrier for clearing the color

	LeVkUtils::TransitionImageLayout(pCommandBuffer, m_swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);


	VkExtent2D swapchainExtent = m_swapchain.GetExtent();

	VkClearValue ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = m_swapchain.GetImageView(pRenderFrame->ImageIndex);
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

LeBool LeVkCore::LeVkRender(LeVkMesh* pMesh)
{
	LE_DEBUG_ASSERT(pMesh);

	LeVkCurrentRenderFrame* pRenderFrame = &m_CurrentRenderFrame[m_FrameIndex];
	VkCommandBuffer CommandBuffer = m_CurrentRenderFrame[m_FrameIndex].CommandBuffer;



	for (uint32 i = 0; i < pMesh->GetNumMatblocks(); i++)
	{
		LeMatBlock* pMb = pMesh->GetMatblock(i);
		LE_DEBUG_ASSERT(pMb);

		LeVkMatBlock* pMbVk = static_cast<LeVkMatBlock*>(pMb);
		LE_DEBUG_ASSERT(pMbVk);

		LeVkMaterial* pMaterialVk = static_cast<LeVkMaterial*>(pMbVk->GetMaterial());

		RefreshGraphicsPipelineForMaterial(pMaterialVk);
		RefreshDescriptorSetForMaterial(pMaterialVk);
		UpdateMaterialTextureImages(CommandBuffer, pMaterialVk);

		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterialVk->GetGraphicsPipeline()->GetPipeline());

		SetupVertexBufferForMatBlock(pMbVk);
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &pMbVk->GetVertexBuffer()->buffer, offsets);

		SetupIndexBufferForMatBlock(pMbVk);
		vkCmdBindIndexBuffer(CommandBuffer, pMbVk->GetIndexBuffer()->buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

		VkDescriptorSet DescriptorSets[] =
		{
			pRenderFrame->DescriptorSet.GetDescriptorSet(),			// First set - global data
			pMesh->GetLeDescriptor()->GetDescriptorSet(),			// Second set - matblock data
			pMaterialVk->GetLeDescriptorSet()->GetDescriptorSet()	// Third set - material data
		};

		uint32 NumDescriptorSets = ARRAY_SIZE(DescriptorSets);

		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterialVk->GetGraphicsPipeline()->GetPipelineLayout(),
			0,						// firstSet
			NumDescriptorSets,		// descriptorSetCount
			DescriptorSets,			// descriptor sets
			0,						// dynamicOffsetCount
			NULL);					// pDynamicOffsets


		vkCmdDrawIndexed(CommandBuffer, uint32(pMbVk->Faces.size()) * 3, 1, 0, 0, 0);
	}

	return true;
}

LeBool LeVkCore::LeVkEndRender(void)
{

	LeVkCurrentRenderFrame* pRenderFrame = &m_CurrentRenderFrame[m_FrameIndex];
	VkCommandBuffer CommandBuffer = m_CurrentRenderFrame[m_FrameIndex].CommandBuffer;

	vkCmdEndRendering(CommandBuffer);


	// transition the image for presenting

	LeVkUtils::TransitionImageLayout(CommandBuffer, m_swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

	EndCommandBuffer(CommandBuffer);

	m_queue.SubmitAsync(CommandBuffer, pRenderFrame->ImageIndex, m_FrameIndex);


	if (!m_queue.Present(pRenderFrame->ImageIndex, m_FrameIndex) || pRenderFrame->bFramebufferResized)
		RecreateSwapChain();

	m_FrameIndex++;
	m_FrameIndex = m_FrameIndex % LE_MAX_FRAMES_IN_FLIGHT;

	return true;
}

VkDevice LeVkCore::LeVkGetLogicalDevice(void)
{
	LE_DEBUG_ASSERT(m_logicalDevice);
	return m_logicalDevice;
}

const LeVkPhysicalDevice* LeVkCore::LeVkGetPhysicalDevice(void)
{
	const LeVkPhysicalDevice* pPhysicalDevice = m_physicalDevices.Selected();
	LE_DEBUG_ASSERT(pPhysicalDevice);

	return pPhysicalDevice;
}

void LeVkCore::CreateCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = m_commandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(m_logicalDevice, &CommandBufferAllocateInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);

}

void LeVkCore::FreeCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(m_logicalDevice, m_commandBufferPool, Count, pCmdBufs);
}


void LeVkCore::BeginCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkCommandBufferBeginInfo CommandBufferBeginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	VkResult res = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
	LE_CHECK_VK_RESULT(res);
}

void LeVkCore::EndCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res);
}

void LeVkCore::CreateOneShotCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{

	VkCommandBufferAllocateInfo cmdBufAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = m_transientCommandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(m_logicalDevice, &cmdBufAllocInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);
}

void LeVkCore::BeginOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
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


void LeVkCore::EndOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res);
}



//static LeVkDescriptor


void LeVkCore::CreateInstance(const char* pAppName)
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
		"VK_MVK_macom_surface",
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
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Vulkan instance created");

}



void LeVkCore::CreateDebugCallback()
{
	VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo{};
	MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	MessengerCreateInfo.messageSeverity =
		//VK_DEBUG_UTILm_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		//VK_DEBUG_UTILm_MESSAGE_SEVERITY_INFO_BIT_EXT	|
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	MessengerCreateInfo.pfnUserCallback = leVkDebugCallback;
	MessengerCreateInfo.pUserData = nullptr;  // Optional

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
	vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	if (!vkCreateDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkCreateDebugUtilsMessenger");
	}

	VkResult res = vkCreateDebugUtilsMessenger(m_instance, &MessengerCreateInfo, NULL, &m_debugMessenger);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Debug utils messenger created");
}

void LeVkCore::CreateSurface(GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pWindow);

	VkResult res = glfwCreateWindowSurface(m_instance, pWindow, NULL, &m_surface);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Window surface created");

}

void LeVkCore::CreateLogicalDevice()
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

	if (m_physicalDevices.Selected()->m_features.geometryShader == VK_FALSE)
		LE_ERROR("The Geometry Shader is not supported!");

	if (m_physicalDevices.Selected()->m_features.tessellationShader == VK_FALSE)
		LE_ERROR("The Tessellation Shader is not supported!");

	if (m_physicalDevices.Selected()->m_features.samplerAnisotropy == VK_FALSE)
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

	VkResult res = vkCreateDevice(m_physicalDevices.Selected()->m_physDevice, &DeviceCreateInfo, NULL, &m_logicalDevice);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Logical device created");

}

VkCommandPool LeVkCore::CreateCommandBufferPool(VkCommandPoolCreateFlags Flags, uint32 QueueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = Flags,
		.queueFamilyIndex = QueueFamilyIndex
	};

	VkCommandPool ret;

	VkResult res = vkCreateCommandPool(m_logicalDevice, &cmdPoolCreateInfo, NULL, &ret);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Command buffer pool created");

	return ret;
}



void LeVkCore::CopyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size)
{

	VkCommandBuffer CopyCommandBuffer;
	CreateOneShotCommandBuffers(1, &CopyCommandBuffer);
	BeginOneShotCommandBuffer(CopyCommandBuffer);


	VkBufferCopy BufferCopy =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Size
	};

	vkCmdCopyBuffer(CopyCommandBuffer, Src, Dst, 1, &BufferCopy);

	EndOneShotCommandBuffer(CopyCommandBuffer);

	// Need to wait because we dealloc buffer memory after this function call >:(
	// FIXME
	m_queue.SubmitSync(CopyCommandBuffer);
	m_queue.WaitIdle();

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



void LeVkCore::SetupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(pVertices);
	LE_DEBUG_ASSERT(Size);
	LE_DEBUG_ASSERT(OutMemoryBuffer);

	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeVkMemoryBuffer StagingVB;
	LeVkUtils::CreateBuffer(m_logicalDevice, &m_physicalDevices.Selected()->m_memProps, Size, Usage, MemProps, &StagingVB);

	// Step 2: map the memory of the stage buffer
	void* pMem = NULL;
	VkDeviceSize Offset = 0;
	VkMemoryMapFlags Flags = 0;
	VkResult res = vkMapMemory(m_logicalDevice, StagingVB.mem, Offset, StagingVB.allocationSize, Flags, &pMem);
	LE_CHECK_VK_RESULT(res);

	// Step 3: copy the vertices to the staging buffer
	LeMemcpy(pMem, pVertices, Size);

	// Step 4: unmap/release the mapped memory
	vkUnmapMemory(m_logicalDevice, StagingVB.mem);

	if (OutMemoryBuffer->allocationSize && Size > OutMemoryBuffer->allocationSize)
	{
		LeVkUtils::DestroyBuffer(OutMemoryBuffer);
	}

	if (!OutMemoryBuffer->allocationSize)
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		LeVkUtils::CreateBuffer(m_logicalDevice, &m_physicalDevices.Selected()->m_memProps, Size, Usage, MemProps, OutMemoryBuffer);
	}
	

	// Step 6: copy the staging buffer to the final buffer
	CopyBuffer(OutMemoryBuffer->buffer, StagingVB.buffer, Size);

	// Step 7: release the resources of the staging buffer
	LeVkUtils::DestroyBuffer(&StagingVB);
}


void LeVkCore::SetupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(pIndices);
	LE_DEBUG_ASSERT(Size);
	LE_DEBUG_ASSERT(OutMemoryBuffer);

	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeVkMemoryBuffer StagingVB;
	LeVkUtils::CreateBuffer(m_logicalDevice, &m_physicalDevices.Selected()->m_memProps, Size, Usage, MemProps, &StagingVB);

	// Step 2: map the memory of the stage buffer
	void* pMem = NULL;
	VkDeviceSize Offset = 0;
	VkMemoryMapFlags Flags = 0;
	VkResult res = vkMapMemory(m_logicalDevice, StagingVB.mem, Offset, StagingVB.allocationSize, Flags, &pMem);
	LE_CHECK_VK_RESULT(res);

	// Step 3: copy the vertices to the staging buffer
	LeMemcpy(pMem, pIndices, Size);

	// Step 4: unmap/release the mapped memory
	vkUnmapMemory(m_logicalDevice, StagingVB.mem);
	if (OutMemoryBuffer->allocationSize && Size > OutMemoryBuffer->allocationSize)
	{
		LeVkUtils::DestroyBuffer(OutMemoryBuffer);
	}

	if (!OutMemoryBuffer->allocationSize)
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		LeVkUtils::CreateBuffer(m_logicalDevice, &m_physicalDevices.Selected()->m_memProps, Size, Usage, MemProps, OutMemoryBuffer);
	}


	// Step 6: copy the staging buffer to the final buffer
	CopyBuffer(OutMemoryBuffer->buffer, StagingVB.buffer, Size);

	// Step 7: release the resources of the staging buffer
	LeVkUtils::DestroyBuffer(&StagingVB);
}

void LeVkCore::RecreateSwapChain(void)
{

	// minimisation case.. TODO obv
	int32 width = 0, height = 0;
	glfwGetFramebufferSize(m_gltfWindow, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_gltfWindow, &width, &height);
		glfwWaitEvents();
	}


	m_queue.WaitIdle();


	m_swapchain.DeInit();
	m_swapchain.Init(m_logicalDevice, m_physicalDevices.Selected(), m_surface, m_queueFamily);
}


// returns true if graphics pipeline changed
// TODO: optimize this
LeBool LeVkCore::RefreshGraphicsPipelineForMaterial(LeVkMaterial* pMaterial)
{

	LE_DEBUG_ASSERT(pMaterial);

	// Test if graphics pipeline needs a refresh

	const LeVkGraphicsPipeline* pGraphicsPipeline = pMaterial->GetGraphicsPipeline();

	LeShader* pShader = pMaterial->GetShader();
	LE_DEBUG_ASSERT(pShader && "Shader is missing material!");

	// Test if pipeline is still valid
	LeGraphicsPipelineVkKey PipelineKey =
	{
		.NumImages = (int32)m_swapchain.GetNumImages(),
		.RenderColFormat = m_swapchain.GetImageFormat(),
		.ShaderHash = pShader->GetNameHash(),
	};

	int64 PipelineKeyHash = lehash64_bin(&PipelineKey, sizeof(PipelineKey));

	if (!pGraphicsPipeline || pGraphicsPipeline->GetKey() != PipelineKeyHash)
	{

		LeVkGraphicsPipeline* FoundGraphicsPipeline =
			LeVkGraphicsPipelineDictionary::Singleton()->Find(PipelineKeyHash);

		if (FoundGraphicsPipeline)
		{
			pMaterial->SetGraphicsPipeline(FoundGraphicsPipeline);

		}
		else
		{
			LeVkShader* pShaderVk = static_cast<LeVkShader*>(pShader);
			LE_DEBUG_ASSERT(pShaderVk);

			LeVkGraphicsPipeline* NewGraphicsPipeline = new LeVkGraphicsPipeline(PipelineKeyHash, m_swapchain.GetNumImages(), pShaderVk, m_swapchain.GetImageFormat());
			LeVkGraphicsPipelineDictionary::Singleton()->Add(NewGraphicsPipeline);

			pMaterial->SetGraphicsPipeline(NewGraphicsPipeline);
		}


		return true;
	}


	return false;
}

LeBool LeVkCore::RefreshDescriptorSetForMaterial(LeVkMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(pMaterial);

	if (pMaterial->HasFlags(MATERIAL_BASE_TEXTURE_DIRTY))
	{
		pMaterial->UpdateDescriptorSet();
		pMaterial->DeleteFlags(MATERIAL_BASE_TEXTURE_DIRTY);

		return true;
	}

	return false;
}

LeBool LeVkCore::UpdateMaterialTextureImages(VkCommandBuffer CommandBuffer, LeVkMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pMaterial);

	LeVkTexture* pBaseTexture = static_cast<LeVkTexture*>(pMaterial->GetBaseTexture());

	if (pBaseTexture && pBaseTexture->HasFlags(TEXTURE_PIXELS_DIRTY))
	{
		UpdateVkImage(CommandBuffer, pBaseTexture);

		pBaseTexture->DeleteFlags(TEXTURE_PIXELS_DIRTY);
	}

	return false;
}

// returns true if buffer was created or updated
LeBool LeVkCore::SetupVertexBufferForMatBlock(LeVkMatBlock* pMbVk)
{
	LE_DEBUG_ASSERT(pMbVk);

	if (pMbVk->HasFlags(MATBLOCK_VERTS_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMbVk->Vertices.size() * sizeof(LeVertex);
		SetupVertexBuffer(pMbVk->Vertices.data(), uLocalSizeOfVerts, pMbVk->GetVertexBuffer()); // this will either create or update the buffer
		pMbVk->DeleteFlags(MATBLOCK_VERTS_DIRTY);

		return true;
	}

	return false;
}

// returns true if buffer was created or updated
LeBool LeVkCore::SetupIndexBufferForMatBlock(LeVkMatBlock* pMbVk)
{
	LE_DEBUG_ASSERT(pMbVk);

	if (pMbVk->HasFlags(MATBLOCK_FACES_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMbVk->Faces.size() * sizeof(LeVertex);
		SetupIndexBuffer(pMbVk->Faces.data(), uLocalSizeOfVerts, pMbVk->GetIndexBuffer()); // this will either create or update the buffer
		pMbVk->DeleteFlags(MATBLOCK_FACES_DIRTY);

		return true;
	}


	return false;

}



void LeVkCore::UpdateVkImage(VkCommandBuffer CommandBuffer, LeVkTexture* pTex)
{

	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pTex);

	int BytesPerPixel = LeVkUtils::GetVkBytesPerTexFormat(pTex->GetVkFormat());

	VkDeviceSize LayerSize = pTex->GetWidth() * pTex->GetHeight() * BytesPerPixel;
	int LayerCount = 1;
	VkDeviceSize ImageSize = LayerCount * LayerSize;

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->textureImage.image,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT);

	copyBufferToImage(CommandBuffer, pTex->textureImage.image, pTex->stagingBuffer.buffer, pTex->GetWidth(), pTex->GetHeight());

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->textureImage.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
}



