
#pragma once

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"

class LeVkQueue
{

public:
	void Init(uint32 QueueFamily, uint32 QueueIndex);
	void DeInit();

	uint32 AcquireNextImage(int32 frameIndex);

	void SubmitSync(VkCommandBuffer CmbBuf);
	void SubmitAsync(VkCommandBuffer CmbBuf, uint32 imageIndex, int32 frameIndex);

	// Swapchain needs to be recreated if returns false
	bool Present(uint32 imageIndex);

	void WaitIdle();

	void WaitForFences(int32 frameIndex);

	void ResetFences(int32 frameIndex);

	VkQueue GetQueue(void) const;

private:


	void CreateSyncObjects(void);
	void FreeSyncObjects(void);


	VkQueue m_hQueue;



	int32 m_NumRenderCompleteSems;
	VkSemaphore* m_phRenderCompleteSems;

	int32 m_NumPresentCompleteSems;
	VkSemaphore* m_phPresentCompleteSems;

	int32 m_NumRenderCompleteFences;
	VkFence* m_phRenderCompleteFences;



	VkDevice m_hDevice;
	LeVkSwapchain* m_hSwapchain;
};