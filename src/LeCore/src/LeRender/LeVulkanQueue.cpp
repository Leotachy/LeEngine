
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeRender/LeRenderUtils.h"


static VkSemaphore createSemaphore(VkDevice Device)
{
	VkSemaphoreCreateInfo CreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};

	VkSemaphore Semaphore;
	VkResult Res = vkCreateSemaphore(Device, &CreateInfo, NULL, &Semaphore);
	LE_CHECK_VK_RESULT(Res, "vkCreateSemaphore");
	return Semaphore;
}


void LeVulkanQueue::Init(VkDevice Device, VkSwapchainKHR SwapChain, uint32 QueueFamily, uint32 QueueIndex)
{
	m_device = Device;
	m_swapChain = SwapChain;

	vkGetDeviceQueue(Device, QueueFamily, QueueIndex, &m_queue);

	LE_DEBUG_MESSAGE("Queue acquired");

	CreateSemaphores();
}


void LeVulkanQueue::Destroy()
{
	vkDestroySemaphore(m_device, m_presentCompleteSem, NULL);
	vkDestroySemaphore(m_device, m_renderCompleteSem, NULL);
}


void LeVulkanQueue::CreateSemaphores()
{
	m_presentCompleteSem = createSemaphore(m_device);
	m_renderCompleteSem = createSemaphore(m_device);
}




void LeVulkanQueue::WaitIdle()
{
	vkQueueWaitIdle(m_queue);
}


uint32 LeVulkanQueue::AcquireNextImage()
{
	uint32 ImageIndex = 0;
	VkResult res = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_presentCompleteSem, NULL, &ImageIndex);
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

	VkResult res = vkQueueSubmit(m_queue, 1, &SubmitInfo, NULL);
	LE_CHECK_VK_RESULT(res, "vkQueueSubmit");
}


void LeVulkanQueue::Present(uint32 ImageIndex)
{
	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_renderCompleteSem,
		.swapchainCount = 1,
		.pSwapchains = &m_swapChain,
		.pImageIndices = &ImageIndex
	};

	VkResult res = vkQueuePresentKHR(m_queue, &PresentInfo);
	LE_CHECK_VK_RESULT(res, "vkQueuePresentKHR");
}
