
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryImage.h"
#include "LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"

#define LE_CHECK_VK_RESULT(res, msg)									\
	if (res != VK_SUCCESS)												\
    {																	\
        LE_DEBUG_ERROR("Bad vulkan result - %s, code %x", msg, res);	\
	}

#define LE_CHECK_VK_RESULT(res)									\
	if (res != VK_SUCCESS)										\
    {															\
        LE_DEBUG_ERROR("Bad vulkan result - code %x", res);		\
	}


void LeVkCreateBuffer(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps,
	VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryBuffer* OutMemoryBuffer);

void LeVkCreateBuffer(VkDevice Device, const VkPhysicalDeviceMemoryProperties * pMemProps,
	VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryBuffer2* OutMemoryBuffer);
void LeVkUpdateBuffer(LeVkMemoryBuffer2 * OutMemoryBuffer, const void* pData, size_t Size);
void LeVkDestroyBuffer(LeVkMemoryBuffer2 * OutMemoryBuffer);

void LeVkCreateImage(VkDevice Device, const VkPhysicalDeviceMemoryProperties * pMemProps,
	uint32 width, uint32 height, VkFormat format,
	VkImageTiling Tiling, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryImage * OutMemoryImage);
void LeVkDestroyImage(LeVkMemoryImage* OutMemoryImage);

void LeVkTransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask);

VkImageView LeVkCreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels);
void LeVkDestroyImageView(VkDevice Device, VkImageView ImageView);

VkSampler LeVkCreateSampler(VkDevice Device, const LeVkTextureAttributes& Attribs, LeBool bEnableAnisotropy = true, float32 fMaxAnisotropy = 1.0f);
void LeVkDestroySampler(VkDevice Device, VkSampler Sampler);

uint32 LeVkGetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * pMemProps,
	uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags);







const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
uint32 GetVkBytesPerTexFormat(VkFormat Format);




