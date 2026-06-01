
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


LeVkRenderer* LeVkRenderer::Get(void)
{
	static LeVkRenderer LeVulkanCore;
	return &LeVulkanCore;
}

void LeVkRenderer::Init(WindowParams* pWindowParams, ViewportParams* pViewportParams, PathParams* pPathParams)
{

	LE_DEBUG_MESSAGE("Initializing LeVkRenderer..");



	LE_DEBUG_ASSERT(pWindowParams);
	LE_DEBUG_ASSERT(pWindowParams->pWindow);
	LE_DEBUG_ASSERT(pWindowParams->pAppName);

	LE_DEBUG_ASSERT(pViewportParams);
	LE_DEBUG_ASSERT(pViewportParams->ViewportWidth > 0);
	LE_DEBUG_ASSERT(pViewportParams->ViewportHeight > 0);

	LE_DEBUG_ASSERT(pPathParams);
	LE_DEBUG_ASSERT(pPathParams->pShaderBinaryPath);

	m_pGltfWindow = pWindowParams->pWindow;

	CreateInstance(pWindowParams->pAppName);
	m_DeletionQueue.AddFunction([this]()
		{
			DestroyInstance();
		});

	CreateDebugCallback();
	m_DeletionQueue.AddFunction([this]()
		{
			DestroyDebugCallback();
		});


	CreateSurface(m_pGltfWindow);
	m_DeletionQueue.AddFunction([this]()
		{
			DestroySurface();
		});

	m_hPhysicalDevices.Init(m_hInstance, m_hSurface);
	m_DeletionQueue.AddFunction([this]()
		{
			m_hPhysicalDevices.DeInit();
		});

	m_QueueFamily = m_hPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);

	CreateLogicalDevice();
	m_DeletionQueue.AddFunction([this]()
		{
			DestroyLogicalDevice();
		});

	CreateVulkanMemoryAllocator();
	m_DeletionQueue.AddFunction([this]()
		{
			DestroyVulkanMemoryAllocator();
		});

	m_Swapchain.Init(m_hSurface, m_QueueFamily);
	m_DeletionQueue.AddFunction([this]()
		{
			m_Swapchain.DeInit();
		});

	m_DescriptorManager.Init();
	m_DeletionQueue.AddFunction([this]()
		{
			m_DescriptorManager.DeInit();
		});

	m_hCommandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_QueueFamily);
	m_hTransientCommandBufferPool = CreateCommandBufferPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_QueueFamily);
	LE_DEBUG_MESSAGE("Command buffer pools created");
	m_DeletionQueue.AddFunction([this]()
		{
			vkDestroyCommandPool(m_hLogicalDevice, m_hTransientCommandBufferPool, NULL);
			vkDestroyCommandPool(m_hLogicalDevice, m_hCommandBufferPool, NULL);

			LE_DEBUG_MESSAGE("Command buffer pools destroyed");
		});

	for (uint32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
	{
		LeVkFrameContext& FrameContext = m_FrameContexts[i];
		CreateCommandBuffers(1, &FrameContext.CommandBuffer);
		FrameContext.ImageIndex = -1;
		FrameContext.FrameUBO.MVP = glm::mat4(1.0f);
		LeVkUtils::CreateUniformBuffer(sizeof(LeVkFrameUniformData), &FrameContext.FrameUBOMemoryBuffer);
	}
	m_FrameIndex = 0;
	m_DeletionQueue.AddFunction([this]()
		{
			for (uint32 i = 0; i < LE_MAX_FRAMES_IN_FLIGHT; i++)
			{
				LeVkFrameContext& FrameContext = m_FrameContexts[i];
				FreeCommandBuffers(1, &FrameContext.CommandBuffer);
				LeVkUtils::DestroyBuffer(&FrameContext.FrameUBOMemoryBuffer);
			}
		});

	m_Queue.Init(m_QueueFamily, 0);
	m_DeletionQueue.AddFunction([this]()
		{
			m_Queue.DeInit();
		});

	m_ShaderDictionary.LoadAllShaders(pPathParams->pShaderBinaryPath);
	m_DeletionQueue.AddFunction([this]()
		{
			m_ShaderDictionary.PurgeAllShaders();
		});

#ifdef SUPPORT_IMGUI
	InitImGui();
	m_DeletionQueue.AddFunction([this]()
		{
			DeInitImGui();
		});
#endif

	m_NewViewportExtent = { 0, 0 };
	m_bRebuildViewportPending = false;
	m_Viewport.Init({ pViewportParams->ViewportWidth, pViewportParams->ViewportHeight });
	m_DeletionQueue.AddFunction([this]()
		{
			m_Viewport.DeInit();
		});

	m_SamplerDictionary.Init();
	m_DeletionQueue.AddFunction([this]()
		{
			m_SamplerDictionary.DeInit();
		});

	m_ObjectCleaner.Init();
	m_DeletionQueue.AddFunction([this]()
		{
			m_ObjectCleaner.DeInit();
		});

	LE_DEBUG_MESSAGE("Finished initializing LeVkRenderer!");
}

void LeVkRenderer::ShutDown(void)
{

	LE_DEBUG_MESSAGE("Shutting down LeVkRenderer..");

	// currently just this.. hack until I sort my world objects
	m_Queue.WaitIdle();

	LE_DEBUG_MESSAGE("Finished shutting down LeVkRenderer");
}

void LeVkRenderer::DeInit(void)
{

	LE_DEBUG_MESSAGE("Deinitializing LeVkRenderer..");

	// DeInit graphics pipelines
	LeVkGraphicsPipelineDictionary::Get()->Purge();

	m_DeletionQueue.Flush();


	LE_DEBUG_MESSAGE("Finished deinitializing LeVkRenderer");

}

LeBool LeVkRenderer::RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	m_Queue.WaitForFences(m_FrameIndex);

	LeVkFrameContext* pRenderFrame = GetFrameContext(m_FrameIndex);

	pRenderFrame->ImageIndex = m_Queue.AcquireNextImage(m_FrameIndex);
	if (pRenderFrame->ImageIndex == -1)
	{
		OnSurfaceSizeChanged();
		return false;
	}

	m_Queue.ResetFences(m_FrameIndex);

	m_DescriptorManager.ResetDescriptors(m_FrameIndex);
	m_ObjectCleaner.Flush(m_FrameIndex);

	if (m_bRebuildViewportPending)
	{
		m_Queue.WaitIdle();
		m_Viewport.ReInit(m_NewViewportExtent);
		m_bRebuildViewportPending = false;
	}

	if (pCam)
		pRenderFrame->FrameUBO.MVP = *pCam->GetViewProjMatrix();
	else
		pRenderFrame->FrameUBO.MVP = glm::mat4(1.0f);

	LeVkUtils::UpdateBuffer(&pRenderFrame->FrameUBOMemoryBuffer, &pRenderFrame->FrameUBO, sizeof(pRenderFrame->FrameUBO));

	LeVkDescriptor* pFrameDescriptor = m_DescriptorManager.GetDescriptor(LeVkDescriptorType::Frame);
	VkDescriptorSet hFrameDescriptorSet = pFrameDescriptor->CreateDescriptorSet(m_FrameIndex, &pRenderFrame->FrameUBOMemoryBuffer);


	VkCommandBuffer CommandBuffer = pRenderFrame->CommandBuffer;
	BeginCommandBuffer(CommandBuffer);

	PreRender(CommandBuffer, pScene);

	// setup image barrier for clearing the color
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_Viewport.GetColorImage(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	LeVkUtils::TransitionImageLayout(CommandBuffer, m_Viewport.GetDepthImage(),
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
	colorAttachment.imageView = m_Viewport.GetColorImageView();
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue = ClearColor;

	VkClearValue ClearDepthStencil = { 1.0f, 0 };

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = m_Viewport.GetDepthImageView();
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.clearValue = ClearDepthStencil;

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = m_Viewport.GetExtent();
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;

	vkCmdBeginRendering(CommandBuffer, &renderingInfo);

	VkViewport Viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float32)m_Viewport.GetExtent().width,
		.height = (float32)m_Viewport.GetExtent().height,
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
			.width = m_Viewport.GetExtent().width,
			.height = m_Viewport.GetExtent().height
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
				hFrameDescriptorSet,										// First set - global data
				pWorldMesh->GetDescriptorSet(),								// Second set - matblock data
				pMaterialVk->GetDescriptorSet()								// Third set - material data
			};

			uint32 NumDescriptorSets = ARRAY_SIZE(DescriptorSets);

			vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterialVk->GetGraphicsPipeline()->GetPipelineLayout(),
				0,						// firstSet
				NumDescriptorSets,		// descriptorSetCount
				DescriptorSets,			// descriptor sets
				0,						// dynamicOffsetCount
				NULL);					// pDynamicOffsets


			vkCmdDrawIndexed(CommandBuffer, pMbVk->GetNumFaces() * 3, 1, 0, 0, 0);
		}
	}

#ifdef SUPPORT_IMGUI
	RenderImGui(CommandBuffer);
#endif

	vkCmdEndRendering(CommandBuffer);

	// Transition the viewport texture for copying from
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_Viewport.GetColorImage(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_2_TRANSFER_READ_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	// Transition the swapchain texture for copying into
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_Swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	// Copy
	LeVkUtils::CopyImageToImage(CommandBuffer, m_Swapchain.GetImage(pRenderFrame->ImageIndex), m_Viewport.GetColorImage(),
		VkExtent3D(m_Swapchain.GetExtent().width, m_Swapchain.GetExtent().height, 1),
		VkExtent3D(m_Viewport.GetExtent().width, m_Viewport.GetExtent().height, 1));
	
	// Transition the swapchain texture for presenting
	LeVkUtils::TransitionImageLayout(CommandBuffer, m_Swapchain.GetImage(pRenderFrame->ImageIndex),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_TRANSFER_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COPY_BIT,
		0,
		VK_IMAGE_ASPECT_COLOR_BIT);

	EndCommandBuffer(CommandBuffer);

	m_Queue.SubmitAsync(CommandBuffer, pRenderFrame->ImageIndex, m_FrameIndex);


	if (!m_Queue.Present(pRenderFrame->ImageIndex, m_FrameIndex) || bFramebufferResized)
		OnSurfaceSizeChanged();

	m_FrameIndex++;
	m_FrameIndex = m_FrameIndex % LE_MAX_FRAMES_IN_FLIGHT;

	return true;
}


LeBool LeVkRenderer::PreRender(VkCommandBuffer CommandBuffer, LeScene* pScene)
{

	LE_DEBUG_ASSERT(CommandBuffer);

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
		UpdateTextureDictionary(CommandBuffer, pTexDictionary);
		pTexDictionary->flags &= ~TEXTURE_DICTIONARY_TEXTURES_DIRTY;
	}
	
	LeMaterialDictionary* pMatDictionary = pScene->GetMaterialDictionary();
	LE_DEBUG_ASSERT(pMatDictionary);

	{
		UpdateMaterialDictionary(pMatDictionary);
	}

	LeVkMesh* pWorldMesh = static_cast<LeVkMesh*>(pScene->GetWorld()->GetMesh());
	LE_DEBUG_ASSERT(pWorldMesh);

	{
		UpdateMesh(CommandBuffer, pWorldMesh);
	}

	return true;
}

void LeVkRenderer::SetViewportDimensions(uint32 width, uint32 height)
{
	m_NewViewportExtent = { width, height };

	if (m_NewViewportExtent.width != m_Viewport.GetExtent().width ||
		m_NewViewportExtent.height != m_Viewport.GetExtent().height)
	{
		m_bRebuildViewportPending = true;
	}
}

#ifdef SUPPORT_IMGUI

ImTextureID LeVkRenderer::GetViewportImGuiTexture(void)
{
	//LE_DEBUG_ASSERT(m_ViewportData.bRenderToImGuiTexture);
	//return (ImTextureID)m_ViewportData.FrameData[m_FrameIndex].ImGuiTexture;

	return 0;
}

#endif

VkDevice LeVkRenderer::GetDevice(void)
{
	LE_DEBUG_ASSERT(m_hLogicalDevice);
	return m_hLogicalDevice;
}

const LeVkPhysicalDevice* LeVkRenderer::GetLePhysicalDevice(void)
{
	const LeVkPhysicalDevice* pPhysicalDevice = m_hPhysicalDevices.Selected();
	LE_DEBUG_ASSERT(pPhysicalDevice);

	return pPhysicalDevice;
}

LeVkSwapchain* LeVkRenderer::GetLeSwapchain(void)
{
	return &m_Swapchain;
}

VmaAllocator LeVkRenderer::GetVmaAllocator(void)
{
	return m_hVmaAllocator;
}

LeVkDescriptorManager* LeVkRenderer::GetDescriptorManager(void)
{
	return &m_DescriptorManager;
}

void LeVkRenderer::CreateCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = m_hCommandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(m_hLogicalDevice, &CommandBufferAllocateInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);

}

void LeVkRenderer::FreeCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(m_hLogicalDevice, m_hCommandBufferPool, Count, pCmdBufs);
}


void LeVkRenderer::BeginCommandBuffer(VkCommandBuffer CommandBuffer)
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

void LeVkRenderer::EndCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res);
}

void LeVkRenderer::CreateOneShotCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers)
{

	VkCommandBufferAllocateInfo cmdBufAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = m_hTransientCommandBufferPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = Count
	};

	VkResult res = vkAllocateCommandBuffers(m_hLogicalDevice, &cmdBufAllocInfo, CommandBuffers);
	LE_CHECK_VK_RESULT(res);
}

void LeVkRenderer::FreeOneShotCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs)
{
	vkFreeCommandBuffers(m_hLogicalDevice, m_hTransientCommandBufferPool, Count, pCmdBufs);
}

void LeVkRenderer::BeginOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
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


void LeVkRenderer::EndOneShotCommandBuffer(VkCommandBuffer CommandBuffer)
{
	VkResult res = vkEndCommandBuffer(CommandBuffer);
	LE_CHECK_VK_RESULT(res);
}



//static LeVkDescriptor


void LeVkRenderer::CreateInstance(const char* pAppName)
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


	VkResult res = vkCreateInstance(&CreateInfo, NULL, &m_hInstance);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Vulkan instance created");

}

void LeVkRenderer::DestroyInstance(void)
{
	vkDestroyInstance(m_hInstance, NULL);
	LE_DEBUG_MESSAGE("Vulkan instance destroyed");
}



void LeVkRenderer::CreateDebugCallback()
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
	vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_hInstance, "vkCreateDebugUtilsMessengerEXT");
	if (!vkCreateDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkCreateDebugUtilsMessenger");
	}

	VkResult res = vkCreateDebugUtilsMessenger(m_hInstance, &MessengerCreateInfo, NULL, &m_hDebugMessenger);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Debug utils messenger created");
}

void LeVkRenderer::DestroyDebugCallback(void)
{
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_hInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (!vkDestroyDebugUtilsMessenger)
	{
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");
	}

	vkDestroyDebugUtilsMessenger(m_hInstance, m_hDebugMessenger, NULL);
	LE_DEBUG_MESSAGE("Vulkan debug utils messenger destroyed");
}

void LeVkRenderer::CreateSurface(GLFWwindow* pWindow)
{
	LE_DEBUG_ASSERT(pWindow);

	VkResult res = glfwCreateWindowSurface(m_hInstance, pWindow, NULL, &m_hSurface);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Window surface created");

}

void LeVkRenderer::DestroySurface(void)
{
	PFN_vkDestroySurfaceKHR vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_hInstance, "vkDestroySurfaceKHR");
	if (!vkDestroySurface)
	{
		LE_ERROR("Cannot find address of vkDestroyDebugUtilsMessenger");
	}

	vkDestroySurfaceKHR(m_hInstance, m_hSurface, NULL);
	LE_DEBUG_MESSAGE("Window surface destroyed");
}

void LeVkRenderer::CreateLogicalDevice()
{
	float qPriorities[] = { 1.0f };

	VkDeviceQueueCreateInfo qInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0, // must be zero
		.queueFamilyIndex = m_QueueFamily,
		.queueCount = 1,
		.pQueuePriorities = &qPriorities[0]
	};

	const char* DevExts[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
		VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
	};

	if (m_hPhysicalDevices.Selected()->Features.geometryShader == VK_FALSE)
		LE_ERROR("The Geometry Shader is not supported!");

	if (m_hPhysicalDevices.Selected()->Features.tessellationShader == VK_FALSE)
		LE_ERROR("The Tessellation Shader is not supported!");

	if (m_hPhysicalDevices.Selected()->Features.samplerAnisotropy == VK_FALSE)
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

	VkResult res = vkCreateDevice(m_hPhysicalDevices.Selected()->hPhysDevice, &DeviceCreateInfo, NULL, &m_hLogicalDevice);
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Logical device created");

}

void LeVkRenderer::DestroyLogicalDevice()
{
	vkDestroyDevice(m_hLogicalDevice, NULL);
	LE_DEBUG_MESSAGE("Logical device destroyed");
}

void LeVkRenderer::CreateVulkanMemoryAllocator(void)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = m_hPhysicalDevices.Selected()->hPhysDevice;
	allocatorInfo.device = m_hLogicalDevice;
	allocatorInfo.instance = m_hInstance;
	allocatorInfo.flags = 0;
	vmaCreateAllocator(&allocatorInfo, &m_hVmaAllocator);

	LE_DEBUG_MESSAGE("Vulkan Memory Allocator created");
}

void LeVkRenderer::DestroyVulkanMemoryAllocator(void)
{
	vmaDestroyAllocator(m_hVmaAllocator);

	LE_DEBUG_MESSAGE("Vulkan Memory Allocator destroyed");
}

#ifdef SUPPORT_IMGUI

void LeVkRenderer::InitImGui(void)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& IO = ImGui::GetIO();
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	IO.DisplaySize.x = float32(m_Swapchain.GetExtent().width);
	IO.DisplaySize.y = float32(m_Swapchain.GetExtent().height);

	ImGui::StyleColorsDark();

	LeBool bInstallGlfwCallbacks = true;
	ImGui_ImplGlfw_InitForVulkan(m_pGltfWindow, bInstallGlfwCallbacks);

	VkFormat ColorAttachmentFormat = m_Swapchain.GetImageFormat();

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.ApiVersion = VK_API_VERSION_1_4;
	init_info.Instance = m_hInstance;
	init_info.PhysicalDevice = m_hPhysicalDevices.Selected()->hPhysDevice;
	init_info.Device = m_hLogicalDevice;
	init_info.QueueFamily = m_QueueFamily;
	init_info.Queue = m_Queue.GetQueue();
	init_info.PipelineCache = NULL;
	init_info.DescriptorPoolSize = 1000;
	init_info.MinImageCount = m_Swapchain.GetMinNumImages();
	init_info.ImageCount = m_Swapchain.GetNumImages();
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

void LeVkRenderer::DeInitImGui(void)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	LE_DEBUG_MESSAGE("ImGui context destroyed");
}

void LeVkRenderer::RenderImGui(VkCommandBuffer CommandBuffer)
{
	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);

	ImDrawData* pDrawData = ImGui::GetDrawData();
	if (pDrawData)
	{
		ImGui_ImplVulkan_RenderDrawData(pDrawData, CommandBuffer);
	}
}

#endif

VkCommandPool LeVkRenderer::CreateCommandBufferPool(VkCommandPoolCreateFlags Flags, uint32 QueueFamilyIndex)
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = Flags,
		.queueFamilyIndex = QueueFamilyIndex
	};

	VkCommandPool ret;

	VkResult res = vkCreateCommandPool(m_hLogicalDevice, &cmdPoolCreateInfo, NULL, &ret);
	LE_CHECK_VK_RESULT(res);

	return ret;
}

void LeVkRenderer::OnSurfaceSizeChanged(void)
{

	// minimisation case.. TODO obv
	int32 width = 0, height = 0;
	glfwGetFramebufferSize(m_pGltfWindow, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_pGltfWindow, &width, &height);
		glfwWaitEvents();
	}

	m_Queue.WaitIdle();


	m_Swapchain.DeInit();
	m_Swapchain.Init(m_hSurface, m_QueueFamily);
}

void LeVkRenderer::UpdateTextureImage(VkCommandBuffer CommandBuffer, LeVkTexture* pTex)
{

	LE_DEBUG_ASSERT(CommandBuffer);
	LE_DEBUG_ASSERT(pTex);

	int32 BytesPerPixel = LeVkUtils::GetVkBytesPerTexFormat(pTex->GetVkFormat());

	VkDeviceSize LayerSize = pTex->GetWidth() * pTex->GetHeight() * BytesPerPixel;
	int32 LayerCount = 1;
	VkDeviceSize ImageSize = LayerCount * LayerSize;

	// Make a staging buffer and fill it with pixel data
	LeVkAllocatedBuffer StagingBuffer;
	LeVkUtils::CreateStagingBuffer(ImageSize, &StagingBuffer);
	LeVkUtils::UpdateBuffer(&StagingBuffer, pTex->GetPixels(), ImageSize);

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->allocatedImage.image,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	LeVkUtils::CopyBufferToImage(CommandBuffer, pTex->allocatedImage.image, StagingBuffer.buffer, VkExtent3D(pTex->GetWidth(), pTex->GetHeight(), 1));

	LeVkUtils::TransitionImageLayout(CommandBuffer, pTex->allocatedImage.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);


	m_ObjectCleaner.PushBuffer(&StagingBuffer, m_FrameIndex);

}

void LeVkRenderer::UpdateTexture(VkCommandBuffer CommandBuffer, LeVkTexture* pTex)
{
	LE_DEBUG_ASSERT(pTex);

	if (pTex->Flags & (TEXTURE_PIXELS_DIRTY))
	{
		UpdateTextureImage(CommandBuffer, pTex);
		pTex->Flags &= ~(TEXTURE_PIXELS_DIRTY);
	}

	if (pTex->Flags & TEXTURE_ATTRIBUTES_DIRTY)
	{
		pTex->sampler = m_SamplerDictionary.GetOrCreateSampler(pTex->GetAttributes());
		pTex->Flags &= ~TEXTURE_ATTRIBUTES_DIRTY;
	}
}

void LeVkRenderer::UpdateTextureDictionary(VkCommandBuffer CommandBuffer, LeTextureDictionary* pTexDictionary)
{

	LE_DEBUG_ASSERT(pTexDictionary);

	pTexDictionary->ForEachTexture([this, CommandBuffer](LeTexture* pTexture)
		{
			LeVkTexture* pVkTexture = static_cast<LeVkTexture*>(pTexture);
			UpdateTexture(CommandBuffer, pVkTexture);
		});
}


LeBool LeVkRenderer::UpdateGraphicsPipelineForMaterial(LeVkMaterial* pMaterial)
{

	LE_DEBUG_ASSERT(pMaterial);

	// Test if graphics pipeline needs a refresh

	const LeVkGraphicsPipeline* pGraphicsPipeline = pMaterial->GetGraphicsPipeline();

	//LeShader* pShader = pMaterial->GetShader();
	//LE_DEBUG_ASSERT(pShader && "Material is missing a shader!");
	LeVkShader* pVertexShader = static_cast<LeVkShader*>(m_ShaderDictionary.FindShader(LeHash::hash64("basic"), LeShaderType::Vertex));
	LeVkShader* pFragShader = static_cast<LeVkShader*>(m_ShaderDictionary.FindShader(LeHash::hash64("basic"), LeShaderType::Fragment));

	// Test if pipeline is still valid
	LeGraphicsPipelineVkKey PipelineKey =
	{
		.VertexShaderHash = pVertexShader->GetNameHash(),
		.FragmentShaderHash = pFragShader->GetNameHash(),
	};

	int64 PipelineKeyHash = LeHash::hash64_bin(&PipelineKey, sizeof(PipelineKey));

	if (!pGraphicsPipeline || pGraphicsPipeline->GetKey() != PipelineKeyHash)
	{

		LeVkGraphicsPipeline* FoundGraphicsPipeline =
			LeVkGraphicsPipelineDictionary::Get()->Find(PipelineKeyHash);

		if (FoundGraphicsPipeline)
		{
			pMaterial->SetGraphicsPipeline(FoundGraphicsPipeline);

		}
		else
		{
			LeVkGraphicsPipeline* NewGraphicsPipeline = new LeVkGraphicsPipeline(PipelineKeyHash, pVertexShader, pFragShader);
			LeVkGraphicsPipelineDictionary::Get()->Add(NewGraphicsPipeline);

			pMaterial->SetGraphicsPipeline(NewGraphicsPipeline);
		}


		return true;
	}


	return false;
}

void LeVkRenderer::UpdateMaterial(LeVkMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(pMaterial);

	if (pMaterial->Flags & MATERIAL_SHADER_DIRTY)
	{
		UpdateGraphicsPipelineForMaterial(pMaterial);
		pMaterial->Flags &= ~MATERIAL_SHADER_DIRTY;
	}

	// Setup the descriptor
	{
		LeVkDescriptor* pDescriptor = m_DescriptorManager.GetDescriptor(LeVkDescriptorType::Material);
		LE_DEBUG_ASSERT(pDescriptor);

		LeVkTexture* pBaseTexture = static_cast<LeVkTexture*>(pMaterial->GetBaseTexture());
		VkSampler hBaseTextureSampler = pBaseTexture ? pBaseTexture->sampler : VK_NULL_HANDLE;
		VkImageView hBaseTextureImageView = pBaseTexture ? pBaseTexture->allocatedImage.imageView : VK_NULL_HANDLE;

		MaterialDescriptorSetParams DescriptorSetParams =
		{
			.BaseTextureInfo =
			{
				.hSampler = hBaseTextureSampler,
				.hImageView = hBaseTextureImageView
			}
		};

		VkDescriptorSet hDescriptorSet = pDescriptor->CreateDescriptorSet(m_FrameIndex, &DescriptorSetParams);
		pMaterial->SetDescriptorSet(hDescriptorSet);
	}
	

}

void LeVkRenderer::UpdateMaterialDictionary(LeMaterialDictionary* pMatDictionary)
{
	LE_DEBUG_ASSERT(pMatDictionary);

	pMatDictionary->ForEachMaterial([this](LeMaterial* pMaterial)
		{
			LeVkMaterial* pVkMaterial = static_cast<LeVkMaterial*>(pMaterial);
			UpdateMaterial(pVkMaterial);
		});
}

void LeVkRenderer::UpdateMatBlock(VkCommandBuffer CommandBuffer, LeVkMatBlock* pMatblock)
{
	LE_DEBUG_ASSERT(pMatblock);

	LE_ASSERT(pMatblock->IsCompiled());

	if (pMatblock->Flags & (MATBLOCK_VERTS_DIRTY))
	{
		uint64 uLocalSizeOfVerts = pMatblock->GetNumVerts() * sizeof(LeVertex);
		LeVkAllocatedBuffer StagingBuffer;
		LeVkUtils::CreateStagingBuffer(uLocalSizeOfVerts, &StagingBuffer);
		LeVkUtils::UpdateBuffer(&StagingBuffer, pMatblock->GetVertexData(), uLocalSizeOfVerts);

		LeVkUtils::CopyBufferToBuffer(CommandBuffer, pMatblock->GetVertexBuffer()->buffer, StagingBuffer.buffer, uLocalSizeOfVerts);

		// push for deletion
		m_ObjectCleaner.PushBuffer(&StagingBuffer, m_FrameIndex);

		pMatblock->Flags &= ~(MATBLOCK_VERTS_DIRTY);
	}

	if (pMatblock->Flags & (MATBLOCK_FACES_DIRTY))
	{
		uint64 uLocalSizeOfFaces = pMatblock->GetNumFaces() * sizeof(LeFace);
		LeVkAllocatedBuffer StagingBuffer;
		LeVkUtils::CreateStagingBuffer(uLocalSizeOfFaces, &StagingBuffer);
		LeVkUtils::UpdateBuffer(&StagingBuffer, pMatblock->GetFaceData(), uLocalSizeOfFaces);

		LeVkUtils::CopyBufferToBuffer(CommandBuffer, pMatblock->GetIndexBuffer()->buffer, StagingBuffer.buffer, uLocalSizeOfFaces);

		// push for deletion
		m_ObjectCleaner.PushBuffer(&StagingBuffer, m_FrameIndex);

		pMatblock->Flags &= ~(MATBLOCK_FACES_DIRTY);
	}
}

void LeVkRenderer::UpdateMesh(VkCommandBuffer CommandBuffer, LeVkMesh* pMesh)
{
	// Setup the descriptor
	{
		LeVkDescriptor* pDescriptor = m_DescriptorManager.GetDescriptor(LeVkDescriptorType::Mesh);
		LE_DEBUG_ASSERT(pDescriptor);

		VkDescriptorSet hDescriptorSet = pDescriptor->CreateDescriptorSet(m_FrameIndex, pMesh->GetModelMatrixUBO());
		pMesh->SetDescriptorSet(hDescriptorSet);
	}

	// Update children matblocks

	for (uint32 i = 0; i < pMesh->GetNumMatblocks(); i++)
	{
		LeMatBlock* pMb = pMesh->GetMatblock(i);
		LE_DEBUG_ASSERT(pMb);

		LeVkMatBlock* pMbVk = static_cast<LeVkMatBlock*>(pMb);
		LE_DEBUG_ASSERT(pMbVk);

		UpdateMatBlock(CommandBuffer, pMbVk);
	}
}

LeVkFrameContext* LeVkRenderer::GetFrameContext(uint32 FrameIndex)
{
	LE_DEBUG_ASSERT(FrameIndex < LE_MAX_FRAMES_IN_FLIGHT);

	return &m_FrameContexts[FrameIndex];
}
