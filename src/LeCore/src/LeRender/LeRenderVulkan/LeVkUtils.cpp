
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"


void LeVkUtils::CreateBuffer(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps,
	VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pMemProps);
	LE_DEBUG_ASSERT(Size > 0);
	LE_DEBUG_ASSERT(OutMemoryBuffer);

	VkBufferCreateInfo BufferCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = Size,
		.usage = Usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	OutMemoryBuffer->device = Device;

	// Step 1: create a buffer
	VkResult res = vkCreateBuffer(Device, &BufferCreateInfo, NULL, &OutMemoryBuffer->buffer);
	LE_CHECK_VK_RESULT(res);

	// Step 2: get the buffer memory requirements
	VkMemoryRequirements MemReqs = {};
	vkGetBufferMemoryRequirements(Device, OutMemoryBuffer->buffer, &MemReqs);

	OutMemoryBuffer->allocationSize = MemReqs.size;

	// Step 3: get the memory type index
	uint32 MemoryTypeIndex = GetMemoryTypeIndex(pMemProps, MemReqs.memoryTypeBits, Properties);

	// Step 4: allocate memory
	VkMemoryAllocateInfo MemAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = MemReqs.size,
		.memoryTypeIndex = MemoryTypeIndex
	};

	res = vkAllocateMemory(Device, &MemAllocInfo, NULL, &OutMemoryBuffer->mem);
	LE_CHECK_VK_RESULT(res);

	// Step 5: bind memory
	res = vkBindBufferMemory(Device, OutMemoryBuffer->buffer, OutMemoryBuffer->mem, 0);
	LE_CHECK_VK_RESULT(res);
}

void LeVkUtils::UpdateBuffer(LeVkMemoryBuffer* OutMemoryBuffer, const void* pData, size_t Size)
{
	LE_DEBUG_ASSERT(OutMemoryBuffer);
	LE_DEBUG_ASSERT(OutMemoryBuffer->device != VK_NULL_HANDLE);


	void* pMem = NULL;
	VkResult res = vkMapMemory(OutMemoryBuffer->device, OutMemoryBuffer->mem, 0, Size, 0, &pMem);
	LE_CHECK_VK_RESULT(res);
	LeMemcpy(pMem, pData, Size);
	vkUnmapMemory(OutMemoryBuffer->device, OutMemoryBuffer->mem);
}

void LeVkUtils::DestroyBuffer(LeVkMemoryBuffer* OutMemoryBuffer)
{
	LE_DEBUG_ASSERT(OutMemoryBuffer);
	LE_DEBUG_ASSERT(OutMemoryBuffer->device != VK_NULL_HANDLE);

	vkDestroyBuffer(OutMemoryBuffer->device, OutMemoryBuffer->buffer, NULL);
	OutMemoryBuffer->buffer = VK_NULL_HANDLE;
	
	vkFreeMemory(OutMemoryBuffer->device, OutMemoryBuffer->mem, NULL);
	OutMemoryBuffer->mem = VK_NULL_HANDLE;
	

	OutMemoryBuffer->allocationSize = 0;

	OutMemoryBuffer->device = VK_NULL_HANDLE;
}

void LeVkUtils::CreateImage(VkDevice Device, const VkPhysicalDeviceMemoryProperties* pMemProps,
	uint32 width, uint32 height, VkFormat format,
	VkImageTiling Tiling, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, LeVkMemoryImage* OutMemoryImage)
{

	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(pMemProps);
	LE_DEBUG_ASSERT(OutMemoryImage);

	// Make the texture image

	VkImageCreateInfo ImageInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = VkExtent3D {.width = width, .height = height, .depth = 1},
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

	OutMemoryImage->device = Device;

	// Step 1: create the image
	VkResult res = vkCreateImage(Device, &ImageInfo, NULL, &OutMemoryImage->image);
	LE_CHECK_VK_RESULT(res);


	// Step 2: get the buffer memory requirements
	VkMemoryRequirements MemReqs = { 0 };
	vkGetImageMemoryRequirements(Device, OutMemoryImage->image, &MemReqs);

	// Step 3: get the memory type index
	uint32 MemoryTypeIndex = GetMemoryTypeIndex(pMemProps, MemReqs.memoryTypeBits, Properties);

	// Step 4: allocate memory
	VkMemoryAllocateInfo MemAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = MemReqs.size,
		.memoryTypeIndex = MemoryTypeIndex
	};

	OutMemoryImage->allocationSize = MemReqs.size;

	res = vkAllocateMemory(Device, &MemAllocInfo, NULL, &OutMemoryImage->mem);
	LE_CHECK_VK_RESULT(res);

	// Step 5: bind memory. This bind is permanent
	res = vkBindImageMemory(Device, OutMemoryImage->image, OutMemoryImage->mem, 0);
	LE_CHECK_VK_RESULT(res);
}

void LeVkUtils::DestroyImage(LeVkMemoryImage* OutMemoryImage)
{
	LE_DEBUG_ASSERT(OutMemoryImage);
	LE_DEBUG_ASSERT(OutMemoryImage->device != VK_NULL_HANDLE);

	vkDestroyImage(OutMemoryImage->device, OutMemoryImage->image, NULL);
	OutMemoryImage->image = VK_NULL_HANDLE;

	vkFreeMemory(OutMemoryImage->device, OutMemoryImage->mem, NULL);
	OutMemoryImage->mem = VK_NULL_HANDLE;

	OutMemoryImage->allocationSize = 0;

	OutMemoryImage->device = VK_NULL_HANDLE;
}

void LeVkUtils::TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask)
{

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
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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

VkImageView LeVkUtils::CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels)
{

	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
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


void LeVkUtils::DestroyImageView(VkDevice Device, VkImageView ImageView)
{
	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(ImageView != VK_NULL_HANDLE);


	vkDestroyImageView(Device, ImageView, NULL);
}

VkSampler LeVkUtils::CreateSampler(VkDevice Device, const LeVkTextureAttributes& Attribs, LeBool bEnableAnisotropy, float32 fMaxAnisotropy)
{

	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);

	VkSamplerCreateInfo SamplerCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = Attribs.MagFilter,
		.minFilter = Attribs.MinFilter,
		.mipmapMode = Attribs.MipmapMode,
		.addressModeU = Attribs.AddressModeU,
		.addressModeV = Attribs.AddressModeV,
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

void LeVkUtils::DestroySampler(VkDevice Device, VkSampler Sampler)
{

	LE_DEBUG_ASSERT(Device != VK_NULL_HANDLE);
	LE_DEBUG_ASSERT(Sampler != VK_NULL_HANDLE);

	vkDestroySampler(Device, Sampler, NULL);
}

uint32 LeVkUtils::GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* pMemProps,
	uint32 MemTypeBitsMask, VkMemoryPropertyFlags ReqMemPropFlags)
{


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
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 4 * sizeof(uint16);
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 4 * sizeof(float32);
	default:
		break;
	}

	return 0;
}


