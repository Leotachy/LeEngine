
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeRender/LeRenderUtils.h"


static VkSemaphore createSemaphore(VkDevice Device)
{
	VkSemaphoreCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	VkSemaphore Semaphore;
	VkResult Res = vkCreateSemaphore(Device, &CreateInfo, NULL, &Semaphore);
	LE_CHECK_VK_RESULT(Res, "vkCreateSemaphore");
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
	VkResult Res = vkCreateFence(Device, &FenceInfo, NULL, &Fence);
	LE_CHECK_VK_RESULT(Res, "vkCreateFence");
	return Fence;
}

LeVulkanQueue::LeVulkanQueue()
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

LeVulkanQueue::~LeVulkanQueue()
{


}

void LeVulkanQueue::Init(VkDevice Device, LeSwapchain* pSwapChain, uint32 QueueFamily, uint32 QueueIndex)
{

	LE_DEBUG_ASSERT(Device);
	LE_DEBUG_ASSERT(pSwapChain);

	m_device = Device;
	m_swapChain = pSwapChain;

	vkGetDeviceQueue(Device, QueueFamily, QueueIndex, &m_queue);

	LE_DEBUG_MESSAGE("Queue acquired");

	CreateSyncObjects();
}

void LeVulkanQueue::Destroy()
{
	FreeSyncObjects();
}

void LeVulkanQueue::WaitIdle()
{
	vkQueueWaitIdle(m_queue);
}

void LeVulkanQueue::WaitForFences(int32 frameIndex)
{
	vkWaitForFences(m_device, 1, &m_renderCompleteFences[frameIndex], true, UINT64_MAX);
}

void LeVulkanQueue::ResetFences(int32 frameIndex)
{
	vkResetFences(m_device, 1, &m_renderCompleteFences[frameIndex]);
}

uint32 LeVulkanQueue::AcquireNextImage(int32 frameIndex)
{
	uint32 ImageIndex = 0;
	VkResult res = vkAcquireNextImageKHR(m_device, m_swapChain->GetSwapchain(), UINT64_MAX, m_presentCompleteSems[frameIndex], NULL, &ImageIndex);
	LE_CHECK_VK_RESULT(res, "vkAcquireNextImageKHR");
	return ImageIndex;
}


void LeVulkanQueue::SubmitAsync(VkCommandBuffer CmbBuf, uint32 imageIndex, int32 frameIndex)
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
	LE_CHECK_VK_RESULT(res, "vkQueueSubmit");
}


void LeVulkanQueue::Present(uint32 ImageIndex)
{

	VkSwapchainKHR Swapchain = m_swapChain->GetSwapchain();

	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderCompleteSems[ImageIndex],
		.swapchainCount = 1,
		.pSwapchains = &Swapchain,
		.pImageIndices = &ImageIndex
	};

	VkResult res = vkQueuePresentKHR(m_queue, &PresentInfo);
	LE_CHECK_VK_RESULT(res, "vkQueuePresentKHR");
}


void LeVulkanQueue::CreateSyncObjects(void)
{


	m_renderCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * m_swapChain->GetNumImages());
	LE_DEBUG_ASSERT(m_renderCompleteSems);
	m_numRenderCompleteSems = m_swapChain->GetNumImages();
	for (int32 i = 0; i < m_numRenderCompleteSems; i++)
		m_renderCompleteSems[i] = createSemaphore(m_device);

	m_presentCompleteSems = (VkSemaphore*)LeMalloc(sizeof(VkSemaphore) * LE_MAX_FRAMES_IN_FLIGHT);
	LE_DEBUG_ASSERT(m_presentCompleteSems);
	m_numPresentCompleteSems = LE_MAX_FRAMES_IN_FLIGHT;
	for (int32 i = 0; i < m_numPresentCompleteSems; i++)
		m_presentCompleteSems[i] = createSemaphore(m_device);

	m_renderCompleteFences = (VkFence*)LeMalloc(sizeof(VkFence) * LE_MAX_FRAMES_IN_FLIGHT);
	LE_DEBUG_ASSERT(m_renderCompleteFences);
	m_numRenderCompleteFences = LE_MAX_FRAMES_IN_FLIGHT;
	for (int32 i = 0; i < m_numRenderCompleteFences; i++)
		m_renderCompleteFences[i] = createFence(m_device);

}

void LeVulkanQueue::FreeSyncObjects(void)
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