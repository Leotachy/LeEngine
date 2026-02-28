

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "window.h"

TestAppWindow::TestAppWindow(int width, int height, const char* name) : m_width{ width }, m_height{ height }, m_name{ name }
{
	Init();
}

TestAppWindow::~TestAppWindow(void)
{
	m_basicShader.Destroy();

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
	m_numImages = m_LeVkCore.GetSwapchain()->GetNumImages();
	m_pRenderQueue = m_LeVkCore.GetQueue();

	m_basicShader.Init(m_LeVkCore.GetDevice(), TESTAPP_SHADER_BIN_PATH "basic.spv");

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

	VkClearValue ClearColor = { 1.0f, 1.0f, 0.0f, 1.0f };

	for (uint32 i = 0; i < m_cmdBufs.size(); i++)
	{
		LE_DEBUG_ASSERT(int32(i) < m_numImages);

		m_LeVkCore.BeginCommandBuffer(m_cmdBufs[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		// setup image barrier for clearing the color

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.image = m_LeVkCore.GetSwapchain()->GetImage(i);
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(
			m_cmdBufs[i],
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);




		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = m_LeVkCore.GetSwapchain()->GetImageView(i);
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.clearValue = ClearColor;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = getExtent();
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;

		vkCmdBeginRendering(m_cmdBufs[i], &renderingInfo);

		vkCmdEndRendering(m_cmdBufs[i]);

		barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = 0;

		vkCmdPipelineBarrier(
			m_cmdBufs[i],
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		m_LeVkCore.EndCommandBuffer(m_cmdBufs[i]);
	}

	LE_DEBUG_MESSAGE("Command buffers recorded\n");
}
