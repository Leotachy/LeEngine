

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "window.h"

TestAppWindow::TestAppWindow(int width, int height, const char* name) : m_width{ width }, m_height{ height }, m_name{ name }
{
	Init();
}

TestAppWindow::~TestAppWindow(void)
{

	m_LeVkCore.FreeCommandBuffers(m_numImages, m_cmdBufs.data());

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void TestAppWindow::Init(void)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(m_width, m_height, m_name, nullptr, nullptr);

	m_LeVkCore.Init(m_name, m_window);
	m_numImages = m_LeVkCore.GetNumImages();
	m_pRenderQueue = m_LeVkCore.GetQueue();

	CreateCommandBuffers();
	RecordCommandBuffers();

}

void TestAppWindow::Draw()
{

	uint32 ImageIndex = m_pRenderQueue->AcquireNextImage();

	m_pRenderQueue->SubmitAsync(m_cmdBufs[ImageIndex]);

	m_pRenderQueue->Present(ImageIndex);

	m_pRenderQueue->WaitIdle();

}

bool TestAppWindow::ShouldClose(void)
{
	return glfwWindowShouldClose(m_window);
}

void TestAppWindow::PollEvents(void)
{
	glfwPollEvents();
}


void TestAppWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
	{
		LE_ERROR("CreateWindowSurface: Failed to create glfwWindowSurface");
		return;
	}


}



void TestAppWindow::CreateCommandBuffers(void)
{
	m_cmdBufs.resize(m_numImages);
	m_LeVkCore.CreateCommandBuffers(m_numImages, m_cmdBufs.data());
}

void TestAppWindow::RecordCommandBuffers(void)
{

	VkClearColorValue ClearColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	VkImageSubresourceRange ImageRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	for (uint32 i = 0; i < m_cmdBufs.size(); i++)
	{

		VkImageMemoryBarrier PresentToClearBarrier = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.pNext = NULL,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = m_LeVkCore.GetImage(i),
				.subresourceRange = ImageRange
		};

		// Change layout of image to be optimal for presenting
		VkImageMemoryBarrier ClearToPresentBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_LeVkCore.GetImage(i),
			.subresourceRange = ImageRange
		};

		m_LeVkCore.BeginCommandBuffer(m_cmdBufs[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);


		vkCmdPipelineBarrier(m_cmdBufs[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,  // dependency flags
			0, NULL, // memory barriers
			0, NULL, // buffer memory barriers
			1, &PresentToClearBarrier); // image memory barriers

		vkCmdClearColorImage(m_cmdBufs[i], m_LeVkCore.GetImage(i), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColor, 1, &ImageRange);

		vkCmdPipelineBarrier(m_cmdBufs[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, NULL, 0, NULL, 1, &ClearToPresentBarrier);

		m_LeVkCore.EndCommandBuffer(m_cmdBufs[i]);
	}

	LE_DEBUG_MESSAGE("Command buffers recorded\n");
}
