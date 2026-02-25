
#pragma once

#include "LeCore/LeCore.h"

class LeVulkanQueue
{

public:
	LeVulkanQueue() {}
	~LeVulkanQueue() {}

	void Init(VkDevice Device, VkSwapchainKHR SwapChain, uint32 QueueFamily, uint32 QueueIndex);

	void Destroy();

	uint32 AcquireNextImage();

	void SubmitSync(VkCommandBuffer CmbBuf);

	void SubmitAsync(VkCommandBuffer CmbBuf);

	void Present(uint32 ImageIndex);

	void WaitIdle();

private:

	void CreateSemaphores();

	VkDevice m_device = VK_NULL_HANDLE;
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	VkQueue m_queue = VK_NULL_HANDLE;
	VkSemaphore m_renderCompleteSem; // post
	VkSemaphore m_presentCompleteSem; // pre
};