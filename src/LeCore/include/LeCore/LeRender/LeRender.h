
#pragma once




#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>



#include "LeCore/LeRender/LePhysicalDevice.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeCore/LeRender/LeSwapchain.h"
#include "LeCore/LeRender/LeShader.h"

#include "glm/glm.hpp"


struct LeVertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
};

const VkVertexInputBindingDescription* LeVertexGetBindingDescription();
const VkVertexInputAttributeDescription* LeVertexGetAttributeDescriptions();
int32 LeVertexGetNumAttributeDescriptions();

class LeMemoryBuffer
{
public:

    VkBuffer m_buffer = NULL;
    VkDeviceMemory m_mem = NULL;
    VkDeviceSize m_allocationSize = 0;

    void Destroy(VkDevice Device)
    {
        if (m_mem)
            vkFreeMemory(Device, m_mem, NULL);

        if (m_buffer)
            vkDestroyBuffer(Device, m_buffer, NULL);
    }
};


constexpr int LE_MAX_FRAMES_IN_FLIGHT = 2;


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

    uint32 GetMemoryTypeIndex(uint32 memTypeBits, VkMemoryPropertyFlags memPropFlags);
    void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties,
        LeMemoryBuffer& OutMemoryBuffer);
    void CopyBuffer(VkBuffer Dst, VkBuffer Src, VkDeviceSize Size);

    GLFWwindow* m_pWindow = NULL;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    LeVulkanPhysicalDevices m_physicalDevices;
    uint32 m_queueFamily = 0;
    VkDevice m_device; // logical device

    LeSwapchain m_swapChain;

    VkCommandPool m_cmdBufPool;
    LeVulkanQueue m_queue;
    VkCommandBuffer m_copyCmdBuf;

public:

   
    void CreateVertexBuffer(const void* pVertices, size_t Size, LeMemoryBuffer& OutMemoryBuffer);

    void CreateCommandBuffers(uint32 Count, VkCommandBuffer* pCmdBufs);
    void FreeCommandBuffers(uint32 Count, const VkCommandBuffer* pCmdBufs);

    void BeginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags);
    void EndCommandBuffer(VkCommandBuffer CommandBuffer);

    LeSwapchain* GetSwapchain(void) { return &m_swapChain; }

    LeVulkanQueue* GetQueue() { return &m_queue; }
    uint32 GetQueueFamily() const { return m_queueFamily; }

    VkDevice GetDevice(void) { return m_device; }

    void RecreateSwapChain(void);

};




