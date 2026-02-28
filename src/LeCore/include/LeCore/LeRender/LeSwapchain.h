

#pragma once

/*
*	Wrapper for VkSwapchainKHR
*/

class LeSwapchain
{

public:


	LeSwapchain();
	~LeSwapchain();

	void Init(VkDevice Device, const LePhysicalDevice* pPhysicalDevice, VkSurfaceKHR Surface, uint32 QueueFamily);
	void Destroy();

	VkSwapchainKHR GetSwapchain(void) { return m_swapChain; }

	uint32 GetNumImages(void) const { return m_numImages; }
	VkImage GetImage(uint32 idx) { return m_images[idx]; }
	VkImageView GetImageView(uint32 idx) { return m_imageViews[idx]; }

	VkFormat GetImageFormat(void) const { return m_imageFormat; }

private:
	VkSwapchainKHR m_swapChain;

	VkImage* m_images;
	VkImageView* m_imageViews;
	uint32 m_numImages;

	VkFormat m_imageFormat;

	VkDevice m_device;

};