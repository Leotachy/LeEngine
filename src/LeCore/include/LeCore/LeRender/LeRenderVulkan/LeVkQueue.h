
#pragma once

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"

class LeVkQueue
{

public:
	void Init(uint32 QueueFamily, uint32 QueueIndex);

	void Destroy();

	uint32 AcquireNextImage(int32 frameIndex);

	void SubmitSync(VkCommandBuffer CmbBuf);
	void SubmitAsync(VkCommandBuffer CmbBuf, uint32 imageIndex, int32 frameIndex);

	// Swapchain needs to be recreated if returns false
	bool Present(uint32 imageIndex, int32 frameIndex);

	void WaitIdle();

	void WaitForFences(int32 frameIndex);

	void ResetFences(int32 frameIndex);

	VkQueue GetQueue(void) const;

private:


	void CreateSyncObjects(void);
	void FreeSyncObjects(void);


	VkQueue m_queue;



	int32 m_numRenderCompleteSems;
	VkSemaphore* m_renderCompleteSems;

	int32 m_numPresentCompleteSems;
	VkSemaphore* m_presentCompleteSems;

	int32 m_numRenderCompleteFences;
	VkFence* m_renderCompleteFences;



	VkDevice m_device;
	LeSwapchain* m_swapChain;
};