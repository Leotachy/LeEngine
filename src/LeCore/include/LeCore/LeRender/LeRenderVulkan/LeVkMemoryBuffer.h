
#pragma once

struct LeVkMemoryBuffer
{

    LeVkMemoryBuffer();
    ~LeVkMemoryBuffer();

    LeBool Alloced(void) const { return buffer != VK_NULL_HANDLE; }
    void DeAlloc(void);

    void Update(const void* pData, size_t Size);

    VkDevice device;

    VkDeviceMemory mem;
    VkBuffer buffer;
    VkDeviceSize allocationSize;

};