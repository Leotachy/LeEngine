
#pragma once




#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>

#include "LeCore/LeRender/LeDevice.h"
#include "LeCore/LeRender/LeVulkanQueue.h"

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
    void CreateSwapChain();
    void CreateCommandBufferPool();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    LeVulkanPhysicalDevices m_physicalDevices;
    uint32 m_queueFamily = 0;
    VkDevice m_device; // logical device
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkCommandPool m_cmdBufPool;
    LeVulkanQueue m_queue;


public:

    int32 GetNumImages(void) { return int32(m_images.size()); }
    VkImage GetImage(uint32 i) { return m_images[i]; }
    void CreateCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
    void FreeCommandBuffers(uint32 Count, const VkCommandBuffer* pCmdBufs);

    void BeginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags);
    void EndCommandBuffer(VkCommandBuffer CommandBuffer);

    LeVulkanQueue* GetQueue() { return &m_queue; }
    uint32 GetQueueFamily() const { return m_queueFamily; }

};

