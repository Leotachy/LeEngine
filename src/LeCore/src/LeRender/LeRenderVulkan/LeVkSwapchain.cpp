

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"

static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes)
{



	for (int i = 0; i < PresentModes.size(); i++)
		if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return PresentModes[i];


	// Fallback to FIFO which is always supported
	return VK_PRESENT_MODE_FIFO_KHR;
}


static uint32 ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	uint32 RequestedNumImages = Capabilities.minImageCount + 1;

	int FinalNumImages = 0;

	if ((Capabilities.maxImageCount > 0) && (RequestedNumImages > Capabilities.maxImageCount))
		FinalNumImages = Capabilities.maxImageCount;
	else
		FinalNumImages = RequestedNumImages;

	return FinalNumImages;
}


static VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats)
{
	for (int i = 0; i < SurfaceFormats.size(); i++)
	{
		if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) &&
			(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
		{
			return SurfaceFormats[i];
		}
	}

	return SurfaceFormats[0];
}

void LeSwapchain::Init(VkSurfaceKHR Surface, uint32 QueueFamily)
{
	VkDevice Device = LeVkRenderer::Get()->GetDevice();
	const LeVkPhysicalDevice* pPhysicalDevice = LeVkRenderer::Get()->GetLePhysicalDevice();

	VkSurfaceCapabilitiesKHR SurfaceCaps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pPhysicalDevice->m_physDevice, Surface, &SurfaceCaps);

	m_extent = SurfaceCaps.currentExtent;

	m_minNumImages = SurfaceCaps.minImageCount;

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
	LE_CHECK_VK_RESULT(res);

	LE_DEBUG_MESSAGE("Swapchain created");

	// make the images also

	uint32 NumSwapChainImages = 0;
	res = vkGetSwapchainImagesKHR(Device, m_swapChain, &NumSwapChainImages, NULL);
	LE_CHECK_VK_RESULT(res);
	LE_DEBUG_ASSERT(NumImages == NumSwapChainImages);

	LE_DEBUG_MESSAGE("Number of swapchain images %d", NumSwapChainImages);

	m_numImages = NumSwapChainImages;

	m_images = (VkImage*)LeMalloc(NumSwapChainImages * sizeof(VkImage));
	LE_DEBUG_ASSERT(m_images);

	m_imageViews = (VkImageView*)LeMalloc(NumSwapChainImages * sizeof(VkImageView));
	LE_DEBUG_ASSERT(m_imageViews);

	res = vkGetSwapchainImagesKHR(Device, m_swapChain, &NumSwapChainImages, m_images);
	LE_CHECK_VK_RESULT(res);

	// And the image views

	int LayerCount = 1;
	int MipLevels = 1;
	for (uint32 i = 0; i < NumSwapChainImages; i++)
	{
		m_imageViews[i] = LeVkUtils::CreateImageView(m_images[i], SurfaceFormat.format,
			VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, LayerCount, MipLevels);
	}
}

void LeSwapchain::DeInit()
{
	VkDevice Device = LeVkRenderer::Get()->GetDevice();

	// DeInit image views

	for (uint32 i = 0; i < m_numImages; i++)
	{
		LeVkUtils::DestroyImageView(m_imageViews[i]);
		m_imageViews[i] = NULL;
	}
		
	m_numImages = 0;
	m_minNumImages = 0;


	// DeInit the swap chain

	vkDestroySwapchainKHR(Device, m_swapChain, NULL);
	m_swapChain = NULL;

	LE_DEBUG_MESSAGE("Swapchain destroyed");
}