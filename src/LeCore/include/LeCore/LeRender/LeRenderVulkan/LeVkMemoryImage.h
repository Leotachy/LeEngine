
#pragma once

struct LeVkMemoryImage
{

    VkDevice device = VK_NULL_HANDLE;

    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    VkDeviceSize allocationSize = 0;
};