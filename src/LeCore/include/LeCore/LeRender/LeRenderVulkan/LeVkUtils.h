
#pragma once

#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"



#define LE_CHECK_VK_RESULT(res)							\
	if (res != VK_SUCCESS)										\
    {															\
        LE_DEBUG_ERROR("Bad vulkan result - code %x", res);		\
	}

class LeVkUtils
{
public:

	static LeVkMemoryBuffer CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	static void UpdateBuffer(LeVkMemoryBuffer* pMemoryBuffer, const void* pData, size_t Size);
	static void DestroyBuffer(LeVkMemoryBuffer* pMemoryBuffer);

	static LeVkMemoryBuffer CreateUniformBuffer(size_t Size);

	static LeVkMemoryImage CreateImage(uint32 width, uint32 height, VkFormat format,
		VkImageTiling Tiling, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties);
	static void DestroyImage(LeVkMemoryImage* OutMemoryImage);
	static void CopyBufferToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkBuffer Src, uint32 width, uint32 height);
	static void CopyImageToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkImage Src, VkExtent2D DstSize, VkExtent2D SrcSize);

	static void TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
		VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
		VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask, VkImageAspectFlags aspectMask);

	static VkImageView CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
		VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels);
	static void DestroyImageView(VkImageView ImageView);

	static VkSampler CreateSampler(const LeVkTextureAttributes* Attribs, LeBool bEnableAnisotropy = true, float32 fMaxAnisotropy = 1.0f);
	static void DestroySampler(VkSampler Sampler);

	static uint32 GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* pMemProps,
		uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags);

	static const LeChar* GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity);
	static const LeChar* GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type);
	static uint32 GetVkBytesPerTexFormat(VkFormat Format);

	static void CheckVkResult(VkResult Res);

};




