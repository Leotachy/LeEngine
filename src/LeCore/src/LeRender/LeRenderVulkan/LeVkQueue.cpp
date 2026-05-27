
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkSwapchain.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkQueue.h"


static VkSemaphore createSemaphore(VkDevice Device)
{
	VkSemaphoreCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	VkSemaphore Semaphore;
	VkResult res = vkCreateSemaphore(Device, &CreateInfo, NULL, &Semaphore);
	LE_CHECK_VK_RESULT(res);
	return Semaphore;
}

static VkFence createFence(VkDevice Device)
{
	VkFenceCreateInfo FenceInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkFence Fence;
	VkResult res = vkCreateFence(Device, &FenceInfo, NULL, &Fence);
	LE_CHECK_VK_RESULT(res);
	return Fence;
}

void LeVkQueue::Init(uint32 QueueFamily, uint32 QueueIndex)
{
	m_hDevice = LeVkRenderer::Get()->GetDevice();
	m_hSwapchain = LeVkRenderer::Get()->GetLeSwapchain();

	vkGetDeviceQueue(m_hDevice, QueueFamily, QueueIndex, &m_hQueue);

	CreateSyncObjects();

	LE_DEBUG_MESSAGE("LeVkQueue initialized at queue family: %u with index: %u", QueueFamily, QueueIndex);
}

void LeVkQueue::DeInit()
{
	FreeSyncObjects();

	LE_DEBUG_MESSAGE("LeVkQueue deinitialized");
}

void LeVkQueue::WaitIdle()
{
	vkQueueWaitIdle(m_hQueue);
}

void LeVkQueue::WaitForFences(int32 frameIndex)
{

	vkWaitForFences(m_hDevice, 1, &m_phRenderCompleteFences[frameIndex], true, UINT64_MAX);
}

void LeVkQueue::ResetFences(int32 frameIndex)
{
	vkResetFences(m_hDevice, 1, &m_phRenderCompleteFences[frameIndex]);
}

VkQueue LeVkQueue::GetQueue(void) const
{
	return m_hQueue;
}

uint32 LeVkQueue::AcquireNextImage(int32 frameIndex)
{
	uint32 ImageIndex = 0;
	VkResult res = vkAcquireNextImageKHR(m_hDevice,
		m_hSwapchain->GetSwapchain(), UINT64_MAX, m_phPresentCompleteSems[frameIndex], NULL, &ImageIndex);

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		return -1; //TODO fix this


	LE_CHECK_VK_RESULT(res);
	return ImageIndex;
}

void LeVkQueue::SubmitSync(VkCommandBuffer CmbBuf)
{
	VkSubmitInfo SubmitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = VK_NULL_HANDLE,
		.pWaitDstStageMask = VK_NULL_HANDLE,
		.commandBufferCount = 1,
		.pCommandBuffers = &CmbBuf,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = VK_NULL_HANDLE
	};

	VkResult res = vkQueueSubmit(m_hQueue, 1, &SubmitInfo, NULL);
	LE_CHECK_VK_RESULT(res);
}

void LeVkQueue::SubmitAsync(VkCommandBuffer CmbBuf, uint32 imageIndex, int32 frameIndex)
{
	VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo SubmitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_phPresentCompleteSems[frameIndex],
		.pWaitDstStageMask = &waitFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &CmbBuf,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_phRenderCompleteSems[imageIndex]
	};

	VkResult res = vkQueueSubmit(m_hQueue, 1, &SubmitInfo, m_phRenderCompleteFences[frameIndex]);
	LE_CHECK_VK_RESULT(res);
}


bool LeVkQueue::Present(uint32 imageIndex, int32 frameIndex)
{

	VkSwapchainKHR Swapchain = m_hSwapchain->GetSwapchain();

	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_phRenderCompleteSems[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &Swapchain,
		.pImageIndices = &imageIndex
	};

	VkResult res = vkQueuePresentKHR(m_hQueue, &PresentInfo);

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		return false; //TODO fix this

	LE_CHECK_VK_RESULT(res);
	return true;
}


void LeVkQueue::CreateSyncObjects(void)
{

	uint32 NumImages = m_hSwapchain->GetNumImages();
	uint32 MaxFramesInFlight = LE_MAX_FRAMES_IN_FLIGHT;

	m_phRenderCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * NumImages);
	LE_DEBUG_ASSERT(m_phRenderCompleteSems);
	m_NumRenderCompleteSems = NumImages;
	for (int32 i = 0; i < m_NumRenderCompleteSems; i++)
		m_phRenderCompleteSems[i] = createSemaphore(m_hDevice);

	m_phPresentCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * MaxFramesInFlight);
	LE_DEBUG_ASSERT(m_phPresentCompleteSems);
	m_NumPresentCompleteSems = MaxFramesInFlight;
	for (int32 i = 0; i < m_NumPresentCompleteSems; i++)
		m_phPresentCompleteSems[i] = createSemaphore(m_hDevice);

	m_phRenderCompleteFences = (VkFence*)LeMalloc(sizeof(VkFence) * MaxFramesInFlight);
	LE_DEBUG_ASSERT(m_phRenderCompleteFences);
	m_NumRenderCompleteFences = MaxFramesInFlight;
	for (int32 i = 0; i < m_NumRenderCompleteFences; i++)
		m_phRenderCompleteFences[i] = createFence(m_hDevice);

}

void LeVkQueue::FreeSyncObjects(void)
{

	for (int32 i = 0; i < m_NumRenderCompleteSems; i++)
		vkDestroySemaphore(m_hDevice, m_phRenderCompleteSems[i], NULL);
	LeFree(m_phRenderCompleteSems);
	m_phRenderCompleteSems = NULL;
	m_NumRenderCompleteSems = 0;



	for (int32 i = 0; i < m_NumPresentCompleteSems; i++)
		vkDestroySemaphore(m_hDevice, m_phPresentCompleteSems[i], NULL);
	LeFree(m_phPresentCompleteSems);
	m_phPresentCompleteSems = NULL;
	m_NumPresentCompleteSems = 0;



	for (int32 i = 0; i < m_NumRenderCompleteFences; i++)
		vkDestroyFence(m_hDevice, m_phRenderCompleteFences[i], NULL);
	LeFree(m_phRenderCompleteFences);
	m_phRenderCompleteFences = NULL;
	m_NumRenderCompleteFences = 0;

	
}