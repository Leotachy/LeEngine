
#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Lukas Andrijauskas
// 10th March, 2026
// Vulkan renderer API implementation loosely based on OGLDEV Vulkan tutorial

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkStagingBufferManager.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetGlobal.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkSamplerDictionary.h"
#include "LeCore/LeShader/LeShaderDictionary.h"
#include "LeCore/LeScene/LeScene.h"


struct LeVkFrameUniformData
{
	glm::mat4 MVP;
};

struct LeVkCurrentRenderFrame
{
	VkCommandBuffer CommandBuffer;
	int32 ImageIndex;
	LeVkFrameUniformData FrameUBO;
	LeVkMemoryBuffer FrameUBOMemoryBuffer;
	LeVkDescriptorSetGlobal DescriptorSet;
};

struct LeVkViewportData
{
	uint32 Width, Height;
	LeBool bRenderToImGuiTexture;
	VkSampler Sampler;

	struct LeVkViewportFrameData
	{
		LeVkMemoryImage OutputImage;
		VkImageView OutputImageView;
#ifdef SUPPORT_IMGUI
		VkDescriptorSet ImGuiTexture;
#endif
	};

	LeVkViewportFrameData FrameData[LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT];

	LeVkMemoryImage DepthImage;
	VkImageView DepthImageView;
};

class LeVkCore
{
public:

	static LeVkCore& Singleton();

	void Init(WindowParams* pWindowParams, ViewportParams* pViewportParams);
	void ShutDown(void);
	void DeInit(void);

	LeBool Render(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

	void SetViewportDimensions(uint32 width, uint32 height);
#ifdef SUPPORT_IMGUI
	ImTextureID GetViewportImGuiTexture(void);
#endif



	VkDevice GetDevice(void);
	const LeVkPhysicalDevice* GetPhysicalDevice(void);
	const LeSwapchain* GetLeSwapchain(void) const;
	int32 GetMaxFramesInFlight(void) const { return LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT; }

private:

	void CreateInstance(const char* pAppName);
	void CreateDebugCallback();
	void CreateSurface(GLFWwindow* pWindow);
	void CreateLogicalDevice();

#ifdef SUPPORT_IMGUI
	void InitImGui(void);
	void DeInitImGui(void);
	void RenderImGui(VkCommandBuffer CommandBuffer);
#endif

	VkCommandPool CreateCommandBufferPool(VkCommandPoolCreateFlags Flags, uint32 QueueFamilyIndex);

	void CreateCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers);
	void FreeCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
	void BeginCommandBuffer(VkCommandBuffer CommandBuffer);
	void EndCommandBuffer(VkCommandBuffer CommandBuffer);

	void CreateOneShotCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers);
	void FreeOneShotCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
	void BeginOneShotCommandBuffer(VkCommandBuffer CommandBuffer);
	void EndOneShotCommandBuffer(VkCommandBuffer CommandBuffer);

	void CopyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size);

	void SetupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer);
	void SetupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer);

	void OnSurfaceSizeChanged(void);
	
	void CreateViewportResources(uint32 width, uint32 height, LeBool bRenderToImGuiTexture);
	void UpdateViewportResources(uint32 width, uint32 height);
	void DestroyViewportResources(void);

	LeBool RefreshGraphicsPipelineForMaterial(LeVkMaterial* pMaterial);
	LeBool RefreshTexture(LeVkTexture* pTexture);
	LeBool RefreshTexturesForMaterial(LeVkMaterial* pMaterial);
	LeBool RefreshDescriptorSetForMaterial(LeVkMaterial* pMaterial);

	LeBool SetupVertexBufferForMatBlock(LeVkMatBlock* pMbVk);
	LeBool SetupIndexBufferForMatBlock(LeVkMatBlock* pMbVk);


	void UpdateTexture(VkCommandBuffer CommandBuffer, LeVkTexture* pTex);
	void UpdateDirtyTextures(VkCommandBuffer CommandBuffer, LeTextureDictionary* pTexDictionary);

	// Records a command buffer of rendering the scene into a viewport image
	LeBool DrawScene(LeScene* pScene, LeCamera* pCam);




	GLFWwindow* m_gltfWindow;

	VkDevice m_logicalDevice;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;
	LeVkPhysicalDevices m_physicalDevices;
	uint32 m_queueFamily;

	LeSwapchain m_swapchain;

	VkCommandPool m_commandBufferPool;
	VkCommandPool m_transientCommandBufferPool; // Command pool for one shot commands
	LeVkQueue m_queue;

	LeVkStagingBufferManager m_stagingBufferManager;

	LeVkCurrentRenderFrame m_CurrentRenderFrame[LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT];

	LeVkViewportData m_ViewportData;
	uint32 m_NewViewportWidth, m_NewViewportHeight;
	LeBool m_bRebuildViewport;

	int32 m_FrameIndex;

	LeVkSamplerDictionary m_samplerDictionary;
	LeShaderDictionary m_shaderDictionary;
};

