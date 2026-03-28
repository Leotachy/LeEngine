
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

struct LeVkMemoryBuffer
{

    LeVkMemoryBuffer();
    ~LeVkMemoryBuffer();

    LeBool Alloced(void) const { return buffer != VK_NULL_HANDLE; }
    void DeAlloc(void);

    void Update(const void* pData, size_t Size);

    VkDevice device = VK_NULL_HANDLE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    VkDeviceSize allocationSize = 0;

};

// New version that's more intuitive
struct LeVkMemoryBuffer2
{

    VkDevice device = VK_NULL_HANDLE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    VkDeviceSize allocationSize = 0;
};