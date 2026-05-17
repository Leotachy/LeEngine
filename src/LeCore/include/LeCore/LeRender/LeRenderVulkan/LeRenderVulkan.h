
#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Lukas Andrijauskas
// 10th March, 2026
// Vulkan renderer API implementation loosely based on OGLDEV Vulkan tutorial

struct LeVkMemoryBuffer
{
	VkBuffer buffer;
	VkDeviceMemory mem;
	VkDeviceSize allocationSize;
};

struct LeVkMemoryImage
{
	VkImage image;
	VkDeviceMemory mem;
	VkDeviceSize allocationSize;
};

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkViewport.h"
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

struct LeVkFrameContext
{
	VkCommandBuffer CommandBuffer;
	int32 ImageIndex;
	LeVkFrameUniformData FrameUBO;
	LeVkMemoryBuffer FrameUBOMemoryBuffer;
	LeVkDescriptorSetGlobal DescriptorSet;
};

class LeVkCore
{
public:



	static LeVkCore& Singleton();

	void Init(WindowParams* pWindowParams, ViewportParams* pViewportParams, PathParams* pPathParams);
	void ShutDown(void);
	void DeInit(void);

	LeBool RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

	void SetViewportDimensions(uint32 width, uint32 height);
#ifdef SUPPORT_IMGUI
	ImTextureID GetViewportImGuiTexture(void);
#endif

	VkDevice GetDevice(void);
	const LeVkPhysicalDevice* GetLePhysicalDevice(void);
	LeSwapchain* GetLeSwapchain(void);

	static const int32 MAX_FRAMES_IN_FLIGHT = 2;
	static const VkFormat VIEWPORT_COLOR_IMAGE_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
	static const VkFormat VIEWPORT_DEPTH_IMAGE_FORMAT = VK_FORMAT_D32_SFLOAT;

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

	void SetupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* pMemoryBuffer);
	void SetupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* pMemoryBuffer);

	void OnSurfaceSizeChanged(void);

	void UpdateTextureImage(LeVkTexture* pTex);
	void UpdateTexture(LeVkTexture* pTex);
	void UpdateTextureDictionary(LeTextureDictionary* pTexDictionary);

	LeBool UpdateGraphicsPipelineForMaterial(LeVkMaterial* pMaterial);
	void UpdateMaterial(LeVkMaterial* pMaterial);

	void UpdateMatBlock(LeVkMatBlock* pMatblock);


	// Records a command buffer of rendering the scene into a viewport image

	LeVkFrameContext* GetFrameContext(uint32 FrameIndex);

	// Runs vulkan updates on the scene
	// e.g. uploading mesh texture pixels to GPU
	LeBool PreRender(LeScene* pScene);



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

	LeVkFrameContext m_FrameContexts[MAX_FRAMES_IN_FLIGHT];


	VkExtent2D m_newViewportExtent;
	LeBool m_bRebuildViewportPending;
	LeVkViewport m_viewport;

	int32 m_FrameIndex;

	LeVkSamplerDictionary m_samplerDictionary;
	LeShaderDictionary m_shaderDictionary;
};

