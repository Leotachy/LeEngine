
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

	static void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VmaAllocationCreateFlags AllocCreateFlags, LeVkAllocatedBuffer* pAllocatedBuffer);
	// Updates the buffer available to CPU
	static void UpdateBuffer(LeVkAllocatedBuffer* pMemoryBuffer, const void* pData, size_t Size);
	static void DestroyBuffer(LeVkAllocatedBuffer* pMemoryBuffer);

	static void CreateUniformBuffer(VkDeviceSize Size, LeVkAllocatedBuffer* pAllocatedBuffer);
	static void CreateStagingBuffer(VkDeviceSize Size, LeVkAllocatedBuffer* pAllocatedBuffer);

	static void CreateImage(VkExtent3D Extent, VkFormat Format,
		VkImageTiling Tiling, VkBufferUsageFlags Usage, VkImageAspectFlags AspectFlags,
		VkImageViewType ViewType, uint32 LayerCount, uint32 MipLevels, LeVkAllocatedImage* pAllocatedBuffer);
	static void DestroyImage(LeVkAllocatedImage* OutMemoryImage);
	static void CopyBufferToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkBuffer Src, VkExtent3D extent);
	static void CopyImageToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkImage Src, VkExtent3D DstSize, VkExtent3D SrcSize);
	static void CopyBufferToBuffer(VkCommandBuffer CommandBuffer, VkBuffer Dst, VkBuffer Src, VkDeviceSize Size);

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




