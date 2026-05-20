
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
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

void LeVkCore::Init(WindowParams* pWindowParams, ViewportParams* pViewportParams, PathParams* pPathParams)
{

	LE_DEBUG_MESSAGE("Initializing LeVkCore..");



	LE_DEBUG_ASSERT(pWindowParams);
	LE_DEBUG_ASSERT(pWindowParams->pWindow);
	LE_DEBUG_ASSERT(pWindowParams->pAppName);

	LE_DEBUG_ASSERT(pViewportParams);
	LE_DEBUG_ASSERT(pViewportParams->uViewportWidth > 0);
	LE_DEBUG_ASSERT(pViewportParams->uViewportHeight > 0);

	LE_DEBUG_ASSERT(pPathParams);
	LE_DEBUG_ASSERT(pPathParams->pShaderBinaryPath);

	m_gltfWindow = pWindowParams->pWindow;

	CreateInstance(pWindowParams->pAppName);
	m_deletionQueue.push_function([this]()
		{
			DestroyInstance();
		});

	CreateDebugCallback();
	m_deletionQueue.push_function([this]()
		{
			DestroyDebugCallback();
		});


	CreateSurface(m_gltfWindow);
	m_deletionQueue.push_function([this]()
		{
			DestroySurface();
		});

	m_physicalDevices.Init(m_instance, m_surface);
	m_deletionQueue.push_function([this]()
		{
			m_physicalDevices.DeInit();
		});


	m_queueFamily = m_physicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);

	CreateLogicalDevice();
	m_deletionQueue.push_function([this]()
		{
			DestroyLogicalDevice();
		});


	m_swapchain.Init(m_surface, m_queueFamily);
	m_deletionQueue.push_function([this]()
		{
			m_swapchain.DeInit();
		});

	LeVkDescriptorPoolManager::Singleton().Init();
	m_deletionQueue.push_function([this]()
		{
			LeVkDescriptorPoolManager::Singleton().DeInit();
		});

	m_commandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_queueFamily);
	m_transientCommandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queueFamily);
	LE_DEBUG_MESSAGE("Command buffer pools created");
	m_deletionQueue.push_function([this]()
		{
			vkDestroyCommandPool(m_logicalDevice, m_transientCommandBufferPool, NULL);
			vkDestroyCommandPool(m_logicalDevice, m_commandBufferPool, NULL);

			LE_DEBUG_MESSAGE("Command buffer pools destroyed");
		});

	for (int32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		LeVkFrameContext& FrameContext = m_FrameContexts[i];
		CreateCommandBuffers(1, &FrameContext.CommandBuffer);
		FrameContext.ImageIndex = -1;
		FrameContext.FrameUBO.MVP = glm::mat4(1.0f);
		FrameContext.FrameUBOMemoryBuffer = LeVkUtils::CreateUniformBuffer(sizeof(LeVkFrameUniformData));
		FrameContext.DescriptorSet.Init();
		FrameContext.DescriptorSet.UpdateSet(&FrameContext.FrameUBOMemoryBuffer);
	}
	m_FrameIndex = 0;
	m_deletionQueue.push_function([this]()
		{
			for (int32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				LeVkFrameContext& FrameContext = m_FrameContexts[i];
				FreeCommandBuffers(1, &FrameContext.CommandBuffer);
				LeVkUtils::DestroyBuffer(&FrameContext.FrameUBOMemoryBuffer);
			}
		});

	m_queue.Init(m_queueFamily, 0);
	m_deletionQueue.push_function([this]()
		{
			m_queue.DeInit();
		});

	m_shaderDictionary.LoadAllShaders(pPathParams->pShaderBinaryPath);
	m_deletionQueue.push_function([this]()
		{
			m_shaderDictionary.PurgeAllShaders();
		});

#ifdef SUPPORT_IMGUI
	InitImGui();
	m_deletionQueue.push_function([this]()
		{
			DeInitImGui();
		});
#endif

	m_newViewportExtent = { 0, 0 };
	m_bRebuildViewportPending = false;
	m_viewport.Init({ pViewportParams->uViewportWidth, pViewportParams->uViewportHeight });
	m_deletionQueue.push_function([this]()
		{
			m_viewport.DeInit();
		});

	m_samplerDictionary.Init();
	m_deletionQueue.push_function([this]()
		{
			m_samplerDictionary.DeInit();
		});

	LE_DEBUG_MESSAGE("Finished initializing LeVkCore!");
}

void LeVkCore::ShutDown(void)
{

	LE_DEBUG_MESSAGE("Shutting down LeVkCore..");

	// currently just this.. hack until I sort my world objects
	m_queue.WaitIdle();

	LE_DEBUG_MESSAGE("Finished shutting down LeVkCore");
}

void LeVkCore::DeInit(void)
{

	LE_DEBUG_MESSAGE("Deinitializing LeVkCore..");

	// DeInit graphics pipelines
	LeVkGraphicsPipelineDictionary::Singleton()->Purge();

	m_deletionQueue.flush();


	LE_DEBUG_MESSAGE("Finished deinitializing LeVkCore");

}

LeBool LeVkCore::RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	m_queue.WaitForFences(m_FrameIndex);

	LeVkFrameContext* pRenderFrame = GetFrameContext(m_FrameIndex);

	pRenderFrame->ImageIndex = m_queue.AcquireNextImage(m_FrameIndex);
	if (pRenderFrame->ImageIndex == -1)
	{
		OnSurfaceSizeChanged();
		return false;
	}

	m_queue.ResetFences(m_FrameIndex);

	if (m_bRebuildViewportPending)
	{
		m_queue.WaitIdle();
		m_viewport.ReInit(m_newViewportExtent);
		m_bRebuildViewportPending = false;
	}

	PreRender(pScene);

	if (pCam)
		pRenderFrame->FrameUBO.MVP = *pCam->GetViewProjMatrix();
	else
		pRenderFrame->FrameUBO.MVP = glm::mat4(1.0f);

	LeVkUtils::UpdateBuffer(&m_FrameContexts[m_FrameIndex].FrameUBOMemoryBuffer,
		&m_FrameContexts[m_FrameIndex].FrameUBO,
		sizeof(m_FrameContexts[m_FrameIndex].FrameUBO));

	VkCommandBuffer CommandBuffer = pRenderFrame->CommandBuffer;
	BeginCommandBuffer(CommandBuffer);

	// setup image barrier for clearing the color
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_viewport.GetColorImage(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	LeVkUtils::TransitionImageLayout(CommandBuffer, m_viewport.GetDepthImage(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT);


	VkClearValue ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = m_viewport.GetColorImageView();
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue = ClearColor;

	VkClearValue ClearDepthStencil = { 1.0f, 0 };

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = m_viewport.GetDepthImageView();
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.clearValue = ClearDepthStencil;

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = m_viewport.GetExtent();
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;

	vkCmdBeginRendering(CommandBuffer, &renderingInfo);

	VkViewport Viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float32)m_viewport.GetExtent().width,
		.height = (float32)m_viewport.GetExtent().height,
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
			.width = m_viewport.GetExtent().width,
			.height = m_viewport.GetExtent().height
		}
	};


	vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
	vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

	if (pScene)
	{
		LeVkMesh* pWorldMesh = static_cast<LeVkMesh*>(pScene->GetWorld()->GetMesh());
		LE_DEBUG_ASSERT(pWorldMesh);

		for (uint32 i = 0; i < pWorldMesh->GetNumMatblocks(); i++)
		{
			LeMatBlock* pMb = pWorldMesh->GetMatblock(i);
			LE_DEBUG_ASSERT(pMb);

			LeVkMatBlock* pMbVk = static_cast<LeVkMatBlock*>(pMb);
			LE_DEBUG_ASSERT(pMbVk);

			LeVkMaterial* pMaterialVk = static_cast<LeVkMaterial*>(pMbVk->GetMaterial());

			vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterialVk->GetGraphicsPipeline()->GetPipeline());

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &pMbVk->GetVertexBuffer()->buffer, offsets);

			vkCmdBindIndexBuffer(CommandBuffer, pMbVk->GetIndexBuffer()->buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

			VkDescriptorSet DescriptorSets[] =
			{
				pRenderFrame->DescriptorSet.GetDescriptorSet(),			// First set - global data
				pWorldMesh->GetLeDescriptor()->GetDescriptorSet(),			// Second set - matblock data
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
	}

#ifdef SUPPORT_IMGUI
	RenderImGui(CommandBuffer);
#endif

	vkCmdEndRendering(CommandBuffer);

	// Transition the viewport texture for copying from
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_viewport.GetColorImage(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_2_TRANSFER_READ_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	// Transition the swapchain texture for copying into
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	// Copy
	LeVkUtils::CopyImageToImage(CommandBuffer, m_swapchain.GetImage(pRenderFrame->ImageIndex),
		m_viewport.GetColorImage(), m_swapchain.GetExtent(), m_viewport.GetExtent());
	
	// Transition the swapchain texture for presenting
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		0,
		VK_IMAGE_ASPECT_COLOR_BIT);

	EndCommandBuffer(CommandBuffer);

	m_queue.SubmitAsync(CommandBuffer, pRenderFrame->ImageIndex, m_FrameIndex);


	if (!m_queue.Present(pRenderFrame->ImageIndex, m_FrameIndex) || bFramebufferResized)
		OnSurfaceSizeChanged();

	m_FrameIndex++;
	m_FrameIndex = m_FrameIndex % MAX_FRAMES_IN_FLIGHT;

	return true;
}


LeBool LeVkCore::PreRender(LeScene* pScene)
{
	if (pScene == NULL)
	{
		return false;
	}

	// Update textures by uploading the pixels to GPU memory
	// Slow but should happen very rarely..
	LeTextureDictionary* pTexDictionary = pScene->GetTextureDictionary();
	LE_DEBUG_ASSERT(pTexDictionary);

	//if (pTexDictionary->flags & TEXTURE_DICTIONARY_TEXTURES_DIRTY)
	{
		UpdateTextureDictionary(pTexDictionary);
		pTexDictionary->flags &= ~TEXTURE_DICTIONARY_TEXTURES_DIRTY;
	}
	
	LeVkMesh* pWorldMesh = static_cast<LeVkMesh*>(pScene->GetWorld()->GetMesh());
	LE_DEBUG_ASSERT(pWorldMesh);

	for (uint32 i = 0; i < pWorldMesh->GetNumMatblocks(); i++)
	{
		LeMatBlock* pMb = pWorldMesh->GetMatblock(i);
		LE_DEBUG_ASSERT(pMb);

		LeVkMatBlock* pMbVk = static_cast<LeVkMatBlock*>(pMb);
		LE_DEBUG_ASSERT(pMbVk);

		UpdateMatBlock(pMbVk);

		LeVkMaterial* pMaterialVk = static_cast<LeVkMaterial*>(pMbVk->GetMaterial());
		LE_DEBUG_ASSERT(pMaterialVk);

		UpdateMaterial(pMaterialVk);
	}

	return true;
}

void LeVkCore::SetViewportDimensions(uint32 width, uint32 height)
{
	m_newViewportExtent = { width, height };

	if (m_newViewportExtent.width != m_viewport.GetExtent().width ||
		m_newViewportExtent.height != m_viewport.GetExtent().height)
	{
		m_bRebuildViewportPending = true;
	}
}

#ifdef SUPPORT_IMGUI

ImTextureID LeVkCore::GetViewportImGuiTexture(void)
{
	//LE_DEBUG_ASSERT(m_ViewportData.bRenderToImGuiTexture);
	//return (ImTextureID)m_ViewportData.FrameData[m_FrameIndex].ImGuiTexture;

	return 0;
}

#endif

VkDevice LeVkCore::GetDevice(void)
{
	LE_DEBUG_ASSERT(m_logicalDevice);
	return m_logicalDevice;
}

const LeVkPhysicalDevice* LeVkCore::GetLePhysicalDevice(void)
{
	const LeVkPhysicalDevice* pPhysicalDevice = m_physicalDevices.Selected();
	LE_DEBUG_ASSERT(pPhysicalDevice);

	return pPhysicalDevice;
}

LeSwapchain* LeVkCore::GetLeSwapchain(void)
{
	return &m_swapchain;
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

void LeVkCore::FreeOneShotCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(m_logicalDevice, m_transientCommandBufferPool, Count, pCmdBufs);
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

void LeVkCore::DestroyInstance(void)
{
	vkDestroyInstance(m_instance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
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

void LeVkCore::DestroyDebugCallback(void)
{
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!vkDestroyDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");
	}

	vkDestroyDebugUtilsMessenger(m_instance, m_debugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");
}

void LeVkCore::CreateSurface(GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pWindow);

	VkResult res = glfwCreateWindowSurface(m_instance, pWindow, NULL, &m_surface);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Window surface created");

}

void LeVkCore::DestroySurface(void)
{
	PFN_vkDestroySurfaceKHR vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
	if (!vkDestroySurface)
	{
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");
	}

	vkDestroySurfaceKHR(m_instance, m_surface, NULL);
	LE_DEBUG_MESSAGE("Window surface destroyed");
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

void LeVkCore::DestroyLogicalDevice()
{
	vkDestroyDevice(m_logicalDevice, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");
}

#ifdef SUPPORT_IMGUI

void LeVkCore::InitImGui(void)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& IO = ImGui::GetIO();
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	IO.DisplaySize.x = float32(m_swapchain.GetExtent().width);
	IO.DisplaySize.y = float32(m_swapchain.GetExtent().height);

	ImGui::StyleColorsDark();

	LeBool bInstallGlfwCallbacks = true;
	ImGui_ImplGlfw_InitForVulkan(m_gltfWindow, bInstallGlfwCallbacks);

	LeVkDescriptorPool* LeDescriptorPool = LeVkDescriptorPoolManager::Singleton().GetLeDescriptorPool(LeVkDescriptorPoolType::IMGUI);
	LE_DEBUG_ASSERT(LeDescriptorPool);

	VkFormat ColorAttachmentFormat = m_swapchain.GetImageFormat();

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.ApiVersion = VK_API_VERSION_1_4;
	init_info.Instance = m_instance;
	init_info.PhysicalDevice = m_physicalDevices.Selected()->m_physDevice;
	init_info.Device = m_logicalDevice;
	init_info.QueueFamily = m_queueFamily;
	init_info.Queue = m_queue.GetQueue();
	init_info.PipelineCache = NULL;
	init_info.DescriptorPool = LeDescriptorPool->GetDescriptorPool();
	init_info.MinImageCount = m_swapchain.GetMinNumImages();
	init_info.ImageCount = m_swapchain.GetNumImages();
	init_info.Allocator = NULL;
	init_info.PipelineInfoMain.RenderPass = NULL;
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.PipelineInfoMain.PipelineRenderingCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.pNext = NULL,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &ColorAttachmentFormat,
		.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
	};
	init_info.UseDynamicRendering = true;
	init_info.CheckVkResultFn = LeVkUtils::CheckVkResult;
	ImGui_ImplVulkan_Init(&init_info);


	LE_DEBUG_MESSAGE("ImGui context created");
}

void LeVkCore::DeInitImGui(void)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	LE_DEBUG_MESSAGE("ImGui context destroyed");
}

void LeVkCore::RenderImGui(VkCommandBuffer CommandBuffer)
{
	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);

	ImDrawData* pDrawData = ImGui::GetDrawData();
	if (pDrawData)
	{
		ImGui_ImplVulkan_RenderDrawData(pDrawData, CommandBuffer);
	}
}

#endif

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

	FreeOneShotCommandBuffers(1, &CopyCommandBuffer);

}

void LeVkCore::SetupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* pMemoryBuffer)
{
	LE_DEBUG_ASSERT(pVertices);
	LE_DEBUG_ASSERT(Size);
	LE_DEBUG_ASSERT(pMemoryBuffer);

	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeVkMemoryBuffer StagingVB = LeVkUtils::CreateBuffer(Size, Usage, MemProps);

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

	if (pMemoryBuffer->allocationSize && Size > pMemoryBuffer->allocationSize)
	{
		LeVkUtils::DestroyBuffer(pMemoryBuffer);
	}

	if (!pMemoryBuffer->allocationSize)
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		*pMemoryBuffer = LeVkUtils::CreateBuffer(Size, Usage, MemProps);
	}
	

	// Step 6: copy the staging buffer to the final buffer
	CopyBuffer(pMemoryBuffer->buffer, StagingVB.buffer, Size);

	// Step 7: release the resources of the staging buffer
	LeVkUtils::DestroyBuffer(&StagingVB);
}


void LeVkCore::SetupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* pMemoryBuffer)
{
	LE_DEBUG_ASSERT(pIndices);
	LE_DEBUG_ASSERT(Size);
	LE_DEBUG_ASSERT(pMemoryBuffer);

	// Step 1: create the staging buffer
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags MemProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	LeVkMemoryBuffer StagingVB = LeVkUtils::CreateBuffer(Size, Usage, MemProps);

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
	if (pMemoryBuffer->allocationSize && Size > pMemoryBuffer->allocationSize)
	{
		LeVkUtils::DestroyBuffer(pMemoryBuffer);
	}

	if (!pMemoryBuffer->allocationSize)
	{
		// Step 5: create the final buffer
		Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		MemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		*pMemoryBuffer = LeVkUtils::CreateBuffer(Size, Usage, MemProps);
	}


	// Step 6: copy the staging buffer to the final buffer
	CopyBuffer(pMemoryBuffer->buffer, StagingVB.buffer, Size);

	// Step 7: release the resources of the staging buffer
	LeVkUtils::DestroyBuffer(&StagingVB);
}

void LeVkCore::OnSurfaceSizeChanged(void)
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
	m_swapchain.Init(m_surface, m_queueFamily);
}

void LeVkCore::UpdateTextureImage(LeVkTexture* pTex)
{

	VkCommandBuffer CommandBuffer;
	CreateOneShotCommandBuffers(1, &CommandBuffer);
	BeginOneShotCommandBuffer(CommandBuffer);

	int32 BytesPerPixel = LeVkUtils::GetVkBytesPerTexFormat(pTex->GetVkFormat());

	VkDeviceSize LayerSize = pTex->GetWidth() * pTex->GetHeight() * BytesPerPixel;
	int32 LayerCount = 1;
	VkDeviceSize ImageSize = LayerCount * LayerSize;

	// Make a staging buffer and fill it with pixel data
	LeVkMemoryBuffer StagingBuffer = LeVkUtils::CreateBuffer(ImageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	LeVkUtils::UpdateBuffer(&StagingBuffer, pTex->GetPixels(), ImageSize);

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->memoryImage.image,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	LeVkUtils::CopyBufferToImage(CommandBuffer, pTex->memoryImage.image, StagingBuffer.buffer, pTex->GetWidth(), pTex->GetHeight());

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->memoryImage.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	EndOneShotCommandBuffer(CommandBuffer);

	m_queue.SubmitSync(CommandBuffer);
	m_queue.WaitIdle();

	LeVkUtils::DestroyBuffer(&StagingBuffer);

	FreeOneShotCommandBuffers(1, &CommandBuffer);
}

void LeVkCore::UpdateTexture(LeVkTexture* pTex)
{
	LE_DEBUG_ASSERT(pTex);

	if (pTex->flags & (TEXTURE_PIXELS_DIRTY))
	{
		UpdateTextureImage(pTex);
		pTex->flags &= ~(TEXTURE_PIXELS_DIRTY);
	}

	if (pTex->flags & TEXTURE_ATTRIBUTES_DIRTY)
	{
		pTex->sampler = m_samplerDictionary.GetOrCreateSampler(pTex->GetAttributes());
		pTex->flags &= ~TEXTURE_ATTRIBUTES_DIRTY;
	}

	/*if (pMaterial->flags & (MATERIAL_BASE_TEXTURE_DIRTY))
	{
		pMaterial->UpdateDescriptorSet();
		pMaterial->flags &= ~(MATERIAL_BASE_TEXTURE_DIRTY);

		return true;
	}*/
}

void LeVkCore::UpdateTextureDictionary(LeTextureDictionary* pTexDictionary)
{

	LE_DEBUG_ASSERT(pTexDictionary);

	pTexDictionary->ForEachTexture([this](LeTexture* pTexture)
		{
			LeVkTexture* pVkTexture = static_cast<LeVkTexture*>(pTexture);
			UpdateTexture(pVkTexture);
		});
}


LeBool LeVkCore::UpdateGraphicsPipelineForMaterial(LeVkMaterial* pMaterial)
{

	LE_DEBUG_ASSERT(pMaterial);

	// Test if graphics pipeline needs a refresh

	const LeVkGraphicsPipeline* pGraphicsPipeline = pMaterial->GetGraphicsPipeline();

	LeShader* pShader = pMaterial->GetShader();
	LE_DEBUG_ASSERT(pShader && "Material is missing a shader!");

	// Test if pipeline is still valid
	LeGraphicsPipelineVkKey PipelineKey =
	{
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

			LeVkGraphicsPipeline* NewGraphicsPipeline = new LeVkGraphicsPipeline(PipelineKeyHash, pShaderVk);
			LeVkGraphicsPipelineDictionary::Singleton()->Add(NewGraphicsPipeline);

			pMaterial->SetGraphicsPipeline(NewGraphicsPipeline);
		}


		return true;
	}


	return false;
}

void LeVkCore::UpdateMaterial(LeVkMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(pMaterial);

	if (pMaterial->flags & MATERIAL_SHADER_DIRTY)
	{
		UpdateGraphicsPipelineForMaterial(pMaterial);
		pMaterial->flags &= ~MATERIAL_SHADER_DIRTY;
	}

	static LeBool deedIsDone = false;
	// Run this every frame until I figure it out
	if (!deedIsDone) pMaterial->UpdateDescriptorSet();

	deedIsDone = true;
}

void LeVkCore::UpdateMatBlock(LeVkMatBlock* pMatblock)
{
	LE_DEBUG_ASSERT(pMatblock);

	if (pMatblock->flags & (MATBLOCK_VERTS_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMatblock->Vertices.size() * sizeof(LeVertex);
		SetupVertexBuffer(pMatblock->Vertices.data(), uLocalSizeOfVerts, pMatblock->GetVertexBuffer()); // this will either create or update the buffer
		pMatblock->flags &= ~(MATBLOCK_VERTS_DIRTY);
	}

	if (pMatblock->flags & (MATBLOCK_FACES_DIRTY))
	{
		uint64 uLocalSizeOfFaces = pMatblock->Faces.size() * sizeof(LeFace);
		SetupIndexBuffer(pMatblock->Faces.data(), uLocalSizeOfFaces, pMatblock->GetIndexBuffer()); // this will either create or update the buffer
		pMatblock->flags &= ~(MATBLOCK_FACES_DIRTY);
	}
}

LeVkFrameContext* LeVkCore::GetFrameContext(uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < MAX_FRAMES_IN_FLIGHT);

	return &m_FrameContexts[FrameIndex];
}
