
#pragma once

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeSwapchain.h"

class LeVulkanQueue
{

public:
	LeVulkanQueue();
	~LeVulkanQueue();

	void Init(VkDevice Device, LeSwapchain* pSwapChain, uint32 QueueFamily, uint32 QueueIndex);

	void Destroy();

	uint32 AcquireNextImage();

	void SubmitSync(VkCommandBuffer CmbBuf);

	void SubmitAsync(VkCommandBuffer CmbBuf);

	void Present(uint32 ImageIndex);

	void WaitIdle();

	void WaitForFences(void);

	void ResetFences(void);

private:

	VkQueue m_queue;
	VkSemaphore m_renderCompleteSem; // post
	VkSemaphore m_presentCompleteSem; // pre
	VkFence m_renderFinishedFence;

	VkDevice m_device;
	LeSwapchain* m_swapChain;
};