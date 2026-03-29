
#pragma once

// New version that's more intuitive
struct LeVkMemoryBuffer
{

    VkDevice device = VK_NULL_HANDLE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    VkDeviceSize allocationSize = 0;
};