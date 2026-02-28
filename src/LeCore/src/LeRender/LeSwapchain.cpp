

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LePhysicalDevice.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeSwapchain.h"

#include "LeRenderUtils.h"


static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes)
{


#ifndef LE_DEBUG
	for (int i = 0; i < PresentModes.size(); i++) {
		if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			return PresentModes[i];
		}
	}
#endif

	// Fallback to FIFO which is always supported
	return VK_PRESENT_MODE_FIFO_KHR;
}


static uint32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	uint32 RequestedNumImages = Capabilities.minImageCount + 1;

	int FinalNumImages = 0;

	if ((Capabilities.maxImageCount > 0) && (RequestedNumImages > Capabilities.maxImageCount)) {
		FinalNumImages = Capabilities.maxImageCount;
	}
	else {
		FinalNumImages = RequestedNumImages;
	}

	return FinalNumImages;
}


static VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats)
{
	for (int i = 0; i < SurfaceFormats.size(); i++) {
		if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			return SurfaceFormats[i];
		}
	}

	return SurfaceFormats[0];
}

static VkImageView CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
	VkImageViewType ViewType, uint32 LayerCount, uint32 mipLevels)
{
	VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
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
	LE_CHECK_VK_RESULT(res, "vkCreateImageView");
	return ImageView;
}

LeSwapchain::LeSwapchain()
{
	m_swapChain = NULL;

	m_images = NULL;
	m_imageViews = NULL;
	m_numImages = 0;

	m_imageFormat = VK_FORMAT_UNDEFINED;

	m_device = NULL;
}
LeSwapchain::~LeSwapchain()
{
}

void LeSwapchain::Init(VkDevice Device, const LePhysicalDevice* pPhysicalDevice,
	VkSurfaceKHR Surface, uint32 QueueFamily)
{

	LE_DEBUG_ASSERT(pPhysicalDevice);

	m_device = Device;

	const VkSurfaceCapabilitiesKHR& SurfaceCaps = pPhysicalDevice->m_surfaceCaps;

	m_extent = SurfaceCaps.currentExtent;

	uint32 NumImages = ChooseNumImages(SurfaceCaps);

	const std::vector<VkPresentModeKHR>& PresentModes = pPhysicalDevice->m_presentModes;
	VkPresentModeKHR PresentMode = ChoosePresentMode(PresentModes);

	VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormatAndColorSpace(pPhysicalDevice->m_surfaceFormats);

	m_imageFormat = SurfaceFormat.format;

	VkSwapchainCreateInfoKHR SwapChainInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = Surface,
		.minImageCount = NumImages,
		.imageFormat = SurfaceFormat.format,
		.imageColorSpace = SurfaceFormat.colorSpace,
		.imageExtent = SurfaceCaps.currentExtent,
		.imageArrayLayers = 1,
		.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &QueueFamily,
		.preTransform = SurfaceCaps.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = PresentMode,
		.clipped = VK_TRUE
	};

	VkResult res = vkCreateSwapchainKHR(Device, &SwapChainInfo, NULL, &m_swapChain);
	LE_CHECK_VK_RESULT(res, "vkCreateSwapchainKHR");

	LE_DEBUG_MESSAGE("Swap chain created");

	// make the images also

	uint32 NumSwapChainImages = 0;
	res = vkGetSwapchainImagesKHR(Device, m_swapChain, &NumSwapChainImages, NULL);
	LE_CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR");
	LE_DEBUG_ASSERT(NumImages == NumSwapChainImages);

	LE_DEBUG_MESSAGE("Number of images %d", NumSwapChainImages);

	m_numImages = NumSwapChainImages;

	m_images = (VkImage*)LeMalloc(NumSwapChainImages * sizeof(VkImage));
	LE_DEBUG_ASSERT(m_images);

	m_imageViews = (VkImageView*)LeMalloc(NumSwapChainImages * sizeof(VkImageView));
	LE_DEBUG_ASSERT(m_imageViews);

	res = vkGetSwapchainImagesKHR(Device, m_swapChain, &NumSwapChainImages, m_images);
	LE_CHECK_VK_RESULT(res, "vkGetSwapchainImagesKHR");

	// And the image views

	int LayerCount = 1;
	int MipLevels = 1;
	for (uint32 i = 0; i < NumSwapChainImages; i++)
	{
		m_imageViews[i] = CreateImageView(Device, m_images[i], SurfaceFormat.format,
			VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, LayerCount, MipLevels);
	}

	LE_DEBUG_MESSAGE("Created image views");
}

void LeSwapchain::Destroy()
{

	// Destroy image views

	for (uint32 i = 0; i < m_numImages; i++)
	{
		vkDestroyImageView(m_device, m_imageViews[i], NULL);
		m_imageViews[i] = NULL;
	}
		
	m_numImages = 0;

	LE_DEBUG_MESSAGE("Image views destroyed");


	// Destroy the swap chain

	vkDestroySwapchainKHR(m_device, m_swapChain, NULL);
	m_swapChain = NULL;

	LE_DEBUG_MESSAGE("Swap chain destroyed");

	// Clear the pointer to device
	m_device = NULL;

}