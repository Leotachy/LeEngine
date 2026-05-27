

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

	VkSwapchainKHR GetSwapchain(void) { return m_hSwapchain; }

	uint32 GetNumImages(void) const { return m_NumImages; }
	uint32 GetMinNumImages(void) const { return m_MinNumImages; }
	VkImage GetImage(uint32 idx) { return m_phImages[idx]; }
	VkImageView GetImageView(uint32 idx) { return m_phImageViews[idx]; }

	VkFormat GetImageFormat(void) const { return m_ImageFormat; }
	VkExtent2D GetExtent(void) const { return m_Extent; }

private:
	VkSwapchainKHR m_hSwapchain;

	VkImage* m_phImages;
	VkImageView* m_phImageViews;

	uint32 m_NumImages;
	uint32 m_MinNumImages;

	VkFormat m_ImageFormat;
	VkExtent2D m_Extent;
};