
#pragma once




#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>

#include "LeCore/LeRender/LePhysicalDevice.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeCore/LeRender/LeSwapchain.h"
#include "LeCore/LeRender/LeShader.h"

class LeVulkanCore
{

public:

    LeVulkanCore();
    ~LeVulkanCore();

    void Init(const char* pAppName, GLFWwindow* pWindow);

private:

    void CreateInstance(const char* pAppName);
    void CreateDebugCallback();
    void CreateSurface(GLFWwindow* pWindow);
    void CreateDevice();
    void CreateCommandBufferPool();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    LeVulkanPhysicalDevices m_physicalDevices;
    uint32 m_queueFamily = 0;
    VkDevice m_device; // logical device

    LeSwapchain m_swapChain;

    VkCommandPool m_cmdBufPool;
    LeVulkanQueue m_queue;

public:

   
    void CreateCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
    void FreeCommandBuffers(uint32 Count, const VkCommandBuffer* pCmdBufs);

    void BeginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags);
    void EndCommandBuffer(VkCommandBuffer CommandBuffer);

    LeSwapchain* GetSwapchain(void) { return &m_swapChain; }

    LeVulkanQueue* GetQueue() { return &m_queue; }
    uint32 GetQueueFamily() const { return m_queueFamily; }

    VkDevice GetDevice(void) { return m_device; }

};

