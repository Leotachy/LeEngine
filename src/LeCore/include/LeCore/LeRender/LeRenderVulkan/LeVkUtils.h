
#pragma once

#define LE_CHECK_VK_RESULT(res, msg)                              \
	if (res != VK_SUCCESS)                                     \
    {                                                          \
        LE_DEBUG_ERROR("Bad vulkan result - %s, code %x", msg, res); \
	}




void LeVkTransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask);

void LeVkCreateCommandBuffers(VkDevice Device, VkCommandPool CommandPool, uint32 count, VkCommandBuffer * cmdBufs);
void LeVkFreeCommandBuffers(VkDevice Device, VkCommandPool CommandPool, uint32 Count, VkCommandBuffer * pCmdBufs);

void LeVkBeginCommandBuffer(VkCommandBuffer CommandBuffer, VkCommandBufferUsageFlags UsageFlags);
void LeVkEndCommandBuffer(VkCommandBuffer CommandBuffer);

uint32 LeVkGetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * pMemProps,
	uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags);

const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
uint32 GetVkBytesPerTexFormat(VkFormat Format);
