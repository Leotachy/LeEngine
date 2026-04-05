
#pragma once

#include <vulkan/vulkan.h>

#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkDescriptor/LeVkDescriptorSetGlobal.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Lukas Andrijauskas
// 10th March, 2026
// Vulkan renderer API implementation loosely based on OGLDEV Vulkan tutorial

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
	LeBool bFramebufferResized;
	LeVkDescriptorSetGlobal DescriptorSet;
};


class LeVkCore
{
public:

	static LeVkCore& Singleton();

	void Init(const char* pAppName, GLFWwindow* pWindow);
	void ShutDown(void);
	void DeInit(void);


	LeBool LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
	LeBool LeVkRender(LeVkMesh* pMesh);
	LeBool LeVkEndRender(void);

	VkDevice LeVkGetLogicalDevice(void);
	const LeVkPhysicalDevice* LeVkGetPhysicalDevice(void);
	int32 GetMaxFramesInFlight(void) const { return LE_MAX_FRAMES_IN_FLIGHT; }

private:

	void CreateInstance(const char* pAppName);
	void CreateDebugCallback();
	void CreateSurface(GLFWwindow* pWindow);
	void CreateLogicalDevice();



	VkCommandPool CreateCommandBufferPool(VkCommandPoolCreateFlags Flags, uint32 QueueFamilyIndex);

	void CreateCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers);
	void FreeCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
	void BeginCommandBuffer(VkCommandBuffer CommandBuffer);
	void EndCommandBuffer(VkCommandBuffer CommandBuffer);

	void CreateOneShotCommandBuffers(uint32 Count, VkCommandBuffer* CommandBuffers);
	void BeginOneShotCommandBuffer(VkCommandBuffer CommandBuffer);
	void EndOneShotCommandBuffer(VkCommandBuffer CommandBuffer);

	void CopyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size);

	void SetupVertexBuffer(const void* pVertices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer);
	void SetupIndexBuffer(const void* pIndices, size_t Size, LeVkMemoryBuffer* OutMemoryBuffer);

	void RecreateSwapChain(void);

	LeBool RefreshGraphicsPipelineForMaterial(LeVkMaterial* pMaterial);
	LeBool RefreshDescriptorSetForMaterial(LeVkMaterial* pMaterial);
	LeBool UpdateMaterialTextureImages(VkCommandBuffer CommandBuffer, LeVkMaterial* pMaterial);
	LeBool SetupVertexBufferForMatBlock(LeVkMatBlock* pMbVk);
	LeBool SetupIndexBufferForMatBlock(LeVkMatBlock* pMbVk);

	void UpdateVkImage(VkCommandBuffer CommandBuffer, LeVkTexture* pTex);





	static const int LE_MAX_FRAMES_IN_FLIGHT = 2;


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


	LeVkCurrentRenderFrame m_CurrentRenderFrame[LE_MAX_FRAMES_IN_FLIGHT];
	int32 m_FrameIndex;
};

