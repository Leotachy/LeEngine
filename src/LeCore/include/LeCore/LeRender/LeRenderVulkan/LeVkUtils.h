
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryBuffer.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkMemoryImage.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"



#define LE_CHECK_VK_RESULT(res)							\
	if (res != VK_SUCCESS)										\
    {															\
        LE_DEBUG_ERROR("Bad vulkan result - code %x", res);		\
	}

class LeVkUtils
{
public:

	static void CreateBuffer(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps,
		VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryBuffer* OutMemoryBuffer);
	static void UpdateBuffer(LeVkMemoryBuffer* OutMemoryBuffer, const void* pData, size_t Size);
	static void DestroyBuffer(LeVkMemoryBuffer* OutMemoryBuffer);

	static void CreateUniformBuffer(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps, 
		size_t Size, LeVkMemoryBuffer* OutMemoryBuffer);

	static void CreateImage(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps,
		uint32 width, uint32 height, VkFormat format,
		VkImageTiling Tiling, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryImage* OutMemoryImage);
	static void DestroyImage(LeVkMemoryImage* OutMemoryImage);

	static void TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
		VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
		VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask);

	static VkImageView CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
		VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels);
	static void DestroyImageView(VkDevice Device, VkImageView ImageView);

	static VkSampler CreateSampler(VkDevice Device, const LeVkTextureAttributes& Attribs, LeBool bEnableAnisotropy = true, float32 fMaxAnisotropy = 1.0f);
	static void DestroySampler(VkDevice Device, VkSampler Sampler);

	static uint32 GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* pMemProps,
		uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags);

	static const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
	static const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
	static uint32 GetVkBytesPerTexFormat(VkFormat Format);

};




