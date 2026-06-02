
#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vk_mem_alloc.h>

#define LE_MAX_FRAMES_IN_FLIGHT 2

struct LeVkAllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
};

struct LeVkAllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
	VkExtent3D extent;
	VkFormat format;
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
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkSamplerDictionary.h"
#include "LeCore/LeShader/LeShaderDictionary.h"
#include "LeCore/LeScene/LeScene.h"
#include "LeCore/LeUtils/LeDeletionQueue.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkObjectCleaner.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPushConstants.h"

struct LeVkFrameUniformData
{
	glm::mat4 MVP;
};

struct LeVkFrameContext
{
	VkCommandBuffer CommandBuffer;
	int32 ImageIndex;
	LeVkFrameUniformData FrameUBO;
	LeVkAllocatedBuffer FrameUBOMemoryBuffer;
	VkDescriptorSet FrameDescriptorSet;
};

class LeVkRenderer
{
public:

	static LeVkRenderer* Get(void);

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
	LeVkSwapchain* GetLeSwapchain(void);
	VmaAllocator GetVmaAllocator(void);
	LeVkDescriptorManager* GetDescriptorManager(void);

	const LeShaderDictionary* GetShaderDictionary(void) { return &m_ShaderDictionary; }

	static const VkFormat VIEWPORT_COLOR_IMAGE_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
	static const VkFormat VIEWPORT_DEPTH_IMAGE_FORMAT = VK_FORMAT_D32_SFLOAT;

private:

	void CreateInstance(const char* pAppName);
	void DestroyInstance(void);

	void CreateDebugCallback(void);
	void DestroyDebugCallback(void);

	void CreateSurface(GLFWwindow* pWindow);
	void DestroySurface(void);

	void CreateLogicalDevice();
	void DestroyLogicalDevice();

	void CreateVulkanMemoryAllocator(void);
	void DestroyVulkanMemoryAllocator(void);


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

	void OnSurfaceSizeChanged(void);

	void UpdateTextureImage(VkCommandBuffer CommandBuffer, LeVkTexture* pTex);
	void UpdateTexture(VkCommandBuffer CommandBuffer, LeVkTexture* pTex);
	void UpdateTextureDictionary(VkCommandBuffer CommandBuffer, LeTextureDictionary* pTexDictionary);

	LeBool UpdateGraphicsPipelineForMaterial(LeVkMaterial* pMaterial);
	void UpdateMaterial(LeVkMaterial* pMaterial);
	void UpdateMaterialDictionary(LeMaterialDictionary* pMatDictionary);


	void UpdateMatBlock(VkCommandBuffer CommandBuffer, LeVkMatBlock* pMatblock);

	void UpdateMesh(VkCommandBuffer CommandBuffer, LeVkMesh* pMesh);

	// Records a command buffer of rendering the scene into a viewport image

	LeVkFrameContext* GetFrameContext(uint32 FrameIndex);

	// Runs vulkan updates on the scene
	// e.g. uploading mesh texture pixels to GPU
	LeBool PreRender(VkCommandBuffer CommandBuffer, LeScene* pScene);




	GLFWwindow* m_pGltfWindow;

	VmaAllocator m_hVmaAllocator;

	VkDevice m_hLogicalDevice;
	VkInstance m_hInstance;
	VkDebugUtilsMessengerEXT m_hDebugMessenger;
	VkSurfaceKHR m_hSurface;
	LeVkPhysicalDevices m_hPhysicalDevices;
	uint32 m_QueueFamily;



	LeVkSwapchain m_Swapchain;

	LeVkDescriptorManager m_DescriptorManager;

	VkCommandPool m_hCommandBufferPool;
	VkCommandPool m_hTransientCommandBufferPool; // Command pool for one shot commands
	LeVkQueue m_Queue;

	LeVkFrameContext m_FrameContexts[LE_MAX_FRAMES_IN_FLIGHT];


	VkExtent2D m_NewViewportExtent;
	LeBool m_bRebuildViewportPending;
	LeVkViewport m_Viewport;

	int32 m_FrameIndex;

	LeVkSamplerDictionary m_SamplerDictionary;
	LeShaderDictionary m_ShaderDictionary;


	LeDeletionQueue m_DeletionQueue;
	LeVkObjectCleaner m_ObjectCleaner;
};

