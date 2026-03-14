
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

struct LeVkMemoryBuffer
{

    LeVkMemoryBuffer();
    ~LeVkMemoryBuffer();

    LeBool Alloced(void) const { return buffer != VK_NULL_HANDLE; }
    void DeAlloc(void);

    void Update(const void* pData, size_t Size);

    VkDevice device;

    VkBuffer buffer;
    VkDeviceMemory mem;
    VkDeviceSize allocationSize;

};