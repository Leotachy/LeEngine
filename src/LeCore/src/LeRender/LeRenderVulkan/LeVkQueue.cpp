
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

LeVkQueue::LeVkQueue()
{
	m_queue = NULL;


	m_numRenderCompleteSems = 0;
	m_renderCompleteSems = NULL;

	m_numPresentCompleteSems = 0;
	m_presentCompleteSems = NULL;

	m_numRenderCompleteFences = 0;
	m_renderCompleteFences = NULL;

	m_device = NULL;
	m_swapChain = NULL;
}

LeVkQueue::~LeVkQueue()
{


}

void LeVkQueue::Init(VkDevice Device, LeSwapchain* pSwapChain, uint32 QueueFamily, uint32 QueueIndex)
{

	LE_DEBUG_ASSERT(Device);
	LE_DEBUG_ASSERT(pSwapChain);

	m_device = Device;
	m_swapChain = pSwapChain;

	vkGetDeviceQueue(Device, QueueFamily, QueueIndex, &m_queue);

	LE_DEBUG_MESSAGE("Queue acquired");

	CreateSyncObjects();
}

void LeVkQueue::Destroy()
{
	FreeSyncObjects();
}

void LeVkQueue::WaitIdle()
{
	vkQueueWaitIdle(m_queue);
}

void LeVkQueue::WaitForFences(int32 frameIndex)
{
	vkWaitForFences(m_device, 1, &m_renderCompleteFences[frameIndex], true, UINT64_MAX);
}

void LeVkQueue::ResetFences(int32 frameIndex)
{
	vkResetFences(m_device, 1, &m_renderCompleteFences[frameIndex]);
}

VkQueue LeVkQueue::GetQueue(void) const
{
	return m_queue;
}

uint32 LeVkQueue::AcquireNextImage(int32 frameIndex)
{
	uint32 ImageIndex = 0;
	VkResult res = vkAcquireNextImageKHR(m_device,
		m_swapChain->GetSwapchain(), UINT64_MAX, m_presentCompleteSems[frameIndex], NULL, &ImageIndex);

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

	VkResult res = vkQueueSubmit(m_queue, 1, &SubmitInfo, NULL);
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
		.pWaitSemaphores = &m_presentCompleteSems[frameIndex],
		.pWaitDstStageMask = &waitFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &CmbBuf,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_renderCompleteSems[imageIndex]
	};

	VkResult res = vkQueueSubmit(m_queue, 1, &SubmitInfo, m_renderCompleteFences[frameIndex]);
	LE_CHECK_VK_RESULT(res);
}


bool LeVkQueue::Present(uint32 imageIndex, int32 frameIndex)
{

	VkSwapchainKHR Swapchain = m_swapChain->GetSwapchain();

	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderCompleteSems[imageIndex],
		.swapchainCount = 1,
		.pSwapchains = &Swapchain,
		.pImageIndices = &imageIndex
	};

	VkResult res = vkQueuePresentKHR(m_queue, &PresentInfo);

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		return false; //TODO fix this

	LE_CHECK_VK_RESULT(res);
	return true;
}


void LeVkQueue::CreateSyncObjects(void)
{

	uint32 NumImages = m_swapChain->GetNumImages();
	uint32 MaxFramesInFlight = LeVkCore::Singleton().GetMaxFramesInFlight();

	m_renderCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * NumImages);
	LE_DEBUG_ASSERT(m_renderCompleteSems);
	m_numRenderCompleteSems = NumImages;
	for (int32 i = 0; i < m_numRenderCompleteSems; i++)
		m_renderCompleteSems[i] = createSemaphore(m_device);

	m_presentCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * MaxFramesInFlight);
	LE_DEBUG_ASSERT(m_presentCompleteSems);
	m_numPresentCompleteSems = MaxFramesInFlight;
	for (int32 i = 0; i < m_numPresentCompleteSems; i++)
		m_presentCompleteSems[i] = createSemaphore(m_device);

	m_renderCompleteFences = (VkFence*)LeMalloc(sizeof(VkFence) * MaxFramesInFlight);
	LE_DEBUG_ASSERT(m_renderCompleteFences);
	m_numRenderCompleteFences = MaxFramesInFlight;
	for (int32 i = 0; i < m_numRenderCompleteFences; i++)
		m_renderCompleteFences[i] = createFence(m_device);

}

void LeVkQueue::FreeSyncObjects(void)
{

	for (int32 i = 0; i < m_numRenderCompleteSems; i++)
		vkDestroySemaphore(m_device, m_renderCompleteSems[i], NULL);
	LeFree(m_renderCompleteSems);
	m_renderCompleteSems = NULL;
	m_numRenderCompleteSems = 0;



	for (int32 i = 0; i < m_numPresentCompleteSems; i++)
		vkDestroySemaphore(m_device, m_presentCompleteSems[i], NULL);
	LeFree(m_presentCompleteSems);
	m_presentCompleteSems = NULL;
	m_numPresentCompleteSems = 0;



	for (int32 i = 0; i < m_numRenderCompleteFences; i++)
		vkDestroyFence(m_device, m_renderCompleteFences[i], NULL);
	LeFree(m_renderCompleteFences);
	m_renderCompleteFences = NULL;
	m_numRenderCompleteFences = 0;

	
}