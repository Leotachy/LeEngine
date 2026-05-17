

#pragma once

/*
*	Wrapper for VkSwapchainKHR
*/

#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"

class LeSwapchain
{
public:
	void Init(VkSurfaceKHR Surface, uint32 QueueFamily);
	void DeInit(void);

	VkSwapchainKHR GetSwapchain(void) { return m_swapChain; }

	uint32 GetNumImages(void) const { return m_numImages; }
	uint32 GetMinNumImages(void) const { return m_minNumImages; }
	VkImage GetImage(uint32 idx) { return m_images[idx]; }
	VkImageView GetImageView(uint32 idx) { return m_imageViews[idx]; }

	VkFormat GetImageFormat(void) const { return m_imageFormat; }
	VkExtent2D GetExtent(void) const { return m_extent; }

private:
	VkSwapchainKHR m_swapChain;

	VkImage* m_images;
	VkImageView* m_imageViews;

	uint32 m_numImages;
	uint32 m_minNumImages;

	VkFormat m_imageFormat;
	VkExtent2D m_extent;
};