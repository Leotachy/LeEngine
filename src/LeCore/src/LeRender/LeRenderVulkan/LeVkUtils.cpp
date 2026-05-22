
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"


#define MARK_VMA_OBJECTS_FOR_DEBUGGING

#ifdef MARK_VMA_OBJECTS_FOR_DEBUGGING

static uint64 s_next_vma_object_mark = 0;

#endif

void LeVkUtils::CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VmaAllocationCreateFlags AllocCreateFlags, LeVkAllocatedBuffer* pAllocatedBuffer)
{
	LE_DEBUG_ASSERT(pAllocatedBuffer);
	LE_DEBUG_ASSERT(Size > 0);

	*pAllocatedBuffer = {};

	VkBufferCreateInfo BufferCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = Size,
		.usage = Usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VmaAllocationCreateInfo AllocationCreateInfo =
	{
		.flags = AllocCreateFlags,
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	VkResult res = vmaCreateBuffer(
		LeVkRenderer::Get()->GetVmaAllocator(), &BufferCreateInfo, &AllocationCreateInfo,
		&pAllocatedBuffer->buffer, &pAllocatedBuffer->allocation, &pAllocatedBuffer->allocationInfo);

#ifdef MARK_VMA_OBJECTS_FOR_DEBUGGING
	vmaSetAllocationUserData(LeVkRenderer::Get()->GetVmaAllocator(), pAllocatedBuffer->allocation, (void*)s_next_vma_object_mark++);
#endif

	LE_CHECK_VK_RESULT(res);
}

void LeVkUtils::UpdateBuffer(LeVkAllocatedBuffer* pMemoryBuffer, const void* pData, size_t Size)
{
	LE_DEBUG_ASSERT(pMemoryBuffer);
	LE_DEBUG_ASSERT(pData);
	LE_DEBUG_ASSERT(Size > 0);

	void* pMappedData = pMemoryBuffer->allocationInfo.pMappedData;
	LeBool bNeedMapping = pMappedData == NULL;

	if (bNeedMapping)
	{
		vmaMapMemory(LeVkRenderer::Get()->GetVmaAllocator(), pMemoryBuffer->allocation, &pMappedData);
	}

	LeMemcpy(pMappedData, pData, Size);

	if (bNeedMapping)
	{
		vmaUnmapMemory(LeVkRenderer::Get()->GetVmaAllocator(), pMemoryBuffer->allocation);
	}
}

void LeVkUtils::DestroyBuffer(LeVkAllocatedBuffer* pAllocatedBuffer)
{
	LE_DEBUG_ASSERT(pAllocatedBuffer);

	vmaDestroyBuffer(LeVkRenderer::Get()->GetVmaAllocator(), pAllocatedBuffer->buffer, pAllocatedBuffer->allocation);
}

void LeVkUtils::CreateUniformBuffer(VkDeviceSize Size, LeVkAllocatedBuffer* pAllocatedBuffer)
{
	LE_DEBUG_ASSERT(Size > 0);
	LE_DEBUG_ASSERT(pAllocatedBuffer);

	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	VmaAllocationCreateFlags AllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	CreateBuffer(Size, Usage, AllocFlags, pAllocatedBuffer);
}

void LeVkUtils::CreateStagingBuffer(VkDeviceSize Size, LeVkAllocatedBuffer* pAllocatedBuffer)
{
	VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VmaAllocationCreateFlags AllocFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
	
	CreateBuffer(Size, Usage, AllocFlags, pAllocatedBuffer);
}

void LeVkUtils::CreateImage(VkExtent3D Extent, VkFormat Format,
	VkImageTiling Tiling, VkBufferUsageFlags Usage, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 MipLevels, LeVkAllocatedImage* pAllocatedImage)
{

	LE_DEBUG_ASSERT(Extent.width > 0 && Extent.height > 0 && Extent.depth > 0);
	LE_DEBUG_ASSERT(pAllocatedImage);

	*pAllocatedImage = {};

	// Make the texture image

	VkImageCreateInfo ImageInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = Format,
		.extent = Extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = Tiling,
		.usage = Usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VmaAllocationCreateInfo AllocationCreateInfo =
	{
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	VkResult res = vmaCreateImage(LeVkRenderer::Get()->GetVmaAllocator(),
		&ImageInfo, &AllocationCreateInfo, &pAllocatedImage->image, &pAllocatedImage->allocation, &pAllocatedImage->allocationInfo);
	LE_CHECK_VK_RESULT(res);

#ifdef MARK_VMA_OBJECTS_FOR_DEBUGGING
	vmaSetAllocationUserData(LeVkRenderer::Get()->GetVmaAllocator(), pAllocatedImage->allocation, (void*)s_next_vma_object_mark++);
#endif

	pAllocatedImage->format = Format;
	pAllocatedImage->extent = Extent;

	pAllocatedImage->imageView = CreateImageView(pAllocatedImage->image, pAllocatedImage->format, AspectFlags, ViewType, LayerCount, MipLevels);
}

void LeVkUtils::DestroyImage(LeVkAllocatedImage* pMemoryImage)
{
	LE_DEBUG_ASSERT(pMemoryImage);

	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	vkDestroyImageView(Device, pMemoryImage->imageView, NULL);

	vmaDestroyImage(LeVkRenderer::Get()->GetVmaAllocator(), pMemoryImage->image, pMemoryImage->allocation);
	
}

void LeVkUtils::TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask, VkImageAspectFlags aspectMask)
{

	LE_DEBUG_ASSERT(cmdBuf != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(image != VK_NULL_HANDLE);


	VkImageMemoryBarrier2 barrier
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessMask,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange =
		{
			.aspectMask = aspectMask,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};


	VkDependencyInfo DependencyInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.dependencyFlags = 0,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
	};

	vkCmdPipelineBarrier2(cmdBuf, &DependencyInfo);
}

VkImageView LeVkUtils::CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels)
{

	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	LE_DEBUG_ASSERT(Image != VK_NULL_HANDLE);

	VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = Image,
		.viewType = ViewType,
		.format = Format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = LayerCount
		}
	};

	VkImageView ImageView;
	VkResult res = vkCreateImageView(Device, &ViewInfo, NULL, &ImageView);
	LE_CHECK_VK_RESULT(res);
	return ImageView;
}


void LeVkUtils::DestroyImageView(VkImageView ImageView)
{
	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	LE_DEBUG_ASSERT(ImageView != VK_NULL_HANDLE);

	vkDestroyImageView(Device, ImageView, NULL);
}

VkSampler LeVkUtils::CreateSampler(const LeVkTextureAttributes* Attribs, LeBool bEnableAnisotropy, float32 fMaxAnisotropy)
{

	LE_DEBUG_ASSERT(Attribs);

	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	VkSamplerCreateInfo SamplerCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = Attribs->MagFilter,
		.minFilter = Attribs->MinFilter,
		.mipmapMode = Attribs->MipmapMode,
		.addressModeU = Attribs->AddressModeU,
		.addressModeV = Attribs->AddressModeV,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.anisotropyEnable = VK_TRUE,
		.maxAnisotropy = fMaxAnisotropy,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};

	VkSampler Sampler;
	VkResult res = vkCreateSampler(Device, &SamplerCreateInfo, NULL, &Sampler);
	LE_CHECK_VK_RESULT(res);


	return Sampler;
}

void LeVkUtils::DestroySampler(VkSampler Sampler)
{

	LE_DEBUG_ASSERT(Sampler != VK_NULL_HANDLE);

	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	vkDestroySampler(Device, Sampler, NULL);
}

void LeVkUtils::CopyBufferToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkBuffer Src, VkExtent3D extent)
{
	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(Dst != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(Src != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(extent.width > 0 && extent.height > 0 && extent.depth > 0);



	VkBufferImageCopy Region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource =
		{
			VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1
		},
		.imageOffset = {0, 0, 0},
		.imageExtent = extent
	};

	vkCmdCopyBufferToImage(CommandBuffer, Src, Dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);
}

void LeVkUtils::CopyImageToImage(VkCommandBuffer CommandBuffer, VkImage Dst, VkImage Src, VkExtent3D DstSize, VkExtent3D SrcSize)
{

	LE_DEBUG_ASSERT(CommandBuffer != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(Dst != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(Src != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(DstSize.width > 0 && DstSize.height > 0 && DstSize.depth > 0);
	LE_DEBUG_ASSERT(SrcSize.width > 0 && SrcSize.height > 0 && DstSize.depth > 0);

	VkImageBlit2 blitRegion = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
		.pNext = NULL
	};

	blitRegion.srcOffsets[1].x = SrcSize.width;
	blitRegion.srcOffsets[1].y = SrcSize.height;
	blitRegion.srcOffsets[1].z = SrcSize.depth;

	blitRegion.dstOffsets[1].x = DstSize.width;
	blitRegion.dstOffsets[1].y = DstSize.height;
	blitRegion.dstOffsets[1].z = DstSize.depth;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
		.pNext = nullptr
	};

	blitInfo.dstImage = Dst;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = Src;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(CommandBuffer, &blitInfo);
}

void LeVkUtils::CopyBufferToBuffer(VkCommandBuffer CommandBuffer, VkBuffer Dst, VkBuffer Src, VkDeviceSize Size)
{

	VkBufferCopy BufferCopy =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = Size
	};

	vkCmdCopyBuffer(CommandBuffer, Src, Dst, 1, &BufferCopy);
}

uint32 LeVkUtils::GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* pMemProps,
	uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
{

	LE_DEBUG_ASSERT(pMemProps);

	for (uint32 i = 0; i < pMemProps->memoryTypeCount; i++)
	{
		uint32 CurBitmask = (1 << i);
		LeBool IsCurMemTypeSupported = (MemTypeBitsMask & CurBitmask);

		if (!IsCurMemTypeSupported)
			continue;

		const VkMemoryType& MemType = pMemProps->memoryTypes[i];
		LeBool HasRequiredMemProps = ((MemType.propertyFlags & ReqMemPropFlags) == ReqMemPropFlags);

		if (!HasRequiredMemProps)
			continue;

		return i;
	}

	LE_DEBUG_ERROR("Cannot find memory type for type %x requested mem props %x", MemTypeBitsMask, ReqMemPropFlags);
	return -1;
}

const LeChar* LeVkUtils::GetVkDebugUtilsMessageSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT Severity)
{
	switch (Severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";

	default:
		LE_ERROR("Invalid severity code %d\n", Severity);
	}

	return "NO SUCH SEVERITY!";
}


const LeChar* LeVkUtils::GetVkDebugUtilsMessageType(VkDebugUtilsMessageTypeFlagsEXT Type)
{
	switch (Type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";

	case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
		return "Device address binding";

	default:
		LE_ERROR("Invalid type code %d\n", Type);
		exit(1);
	}

	return "NO SUCH TYPE!";
}


uint32_t LeVkUtils::GetVkBytesPerTexFormat(VkFormat Format)
{
	switch (Format)
	{
	case VK_FORMAT_R8_SINT:
	case VK_FORMAT_R8_UNORM:
		return 1;
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R16G16_SFLOAT:
		return 4;
	case VK_FORMAT_R16G16_SNORM:
		return 4;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_UINT:
		return 4;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 4 * sizeof(uint16);
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4 * sizeof(float32);
	default:
		break;
	}

	return 0;
}

void LeVkUtils::CheckVkResult(VkResult Res)
{
	LE_CHECK_VK_RESULT(Res);
}

