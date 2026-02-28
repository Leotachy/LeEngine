
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeRender/LeRenderUtils.h"


static VkSemaphore createSemaphore(VkDevice Device)
{
	VkSemaphoreCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
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
	m_renderCompleteSem = NULL;
	m_presentCompleteSem = NULL;
	m_renderFinishedFence = NULL;

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

	m_presentCompleteSem = createSemaphore(m_device);
	m_renderCompleteSem = createSemaphore(m_device);
	m_renderFinishedFence = createFence(m_device);
}


void LeVulkanQueue::Destroy()
{
	vkDestroySemaphore(m_device, m_presentCompleteSem, NULL);
	vkDestroySemaphore(m_device, m_renderCompleteSem, NULL);
	vkDestroyFence(m_device, m_renderFinishedFence, NULL);
}




void LeVulkanQueue::WaitIdle()
{
	vkQueueWaitIdle(m_queue);
}

void LeVulkanQueue::WaitForFences(void)
{
	vkWaitForFences(m_device, 1, &m_renderFinishedFence, true, UINT64_MAX);
}

void LeVulkanQueue::ResetFences(void)
{
	vkResetFences(m_device, 1, &m_renderFinishedFence);
}

uint32 LeVulkanQueue::AcquireNextImage()
{
	uint32 ImageIndex = 0;
	VkResult res = vkAcquireNextImageKHR(m_device, m_swapChain->GetSwapchain(), UINT64_MAX, m_presentCompleteSem, NULL, &ImageIndex);
	LE_CHECK_VK_RESULT(res, "vkAcquireNextImageKHR");
	return ImageIndex;
}


void LeVulkanQueue::SubmitSync(VkCommandBuffer CmbBuf)
{
	VkSubmitInfo SubmitInfo = {
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
	LE_CHECK_VK_RESULT(res, "vkQueueSubmit");
}


void LeVulkanQueue::SubmitAsync(VkCommandBuffer CmbBuf)
{
	VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo SubmitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_presentCompleteSem,
		.pWaitDstStageMask = &waitFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &CmbBuf,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_renderCompleteSem
	};

	VkResult res = vkQueueSubmit(m_queue, 1, &SubmitInfo, m_renderFinishedFence);
	LE_CHECK_VK_RESULT(res, "vkQueueSubmit");
}


void LeVulkanQueue::Present(uint32 ImageIndex)
{

	VkSwapchainKHR Swapchain = m_swapChain->GetSwapchain();

	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderCompleteSem,
		.swapchainCount = 1,
		.pSwapchains = &Swapchain,
		.pImageIndices = &ImageIndex
	};

	VkResult res = vkQueuePresentKHR(m_queue, &PresentInfo);
	LE_CHECK_VK_RESULT(res, "vkQueuePresentKHR");
}
