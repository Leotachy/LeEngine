

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "window.h"

TestAppWindow::TestAppWindow(int width, int height, const char* name) : m_width{ width }, m_height{ height }, m_name{ name }
{
	Init();
}

TestAppWindow::~TestAppWindow(void)
{
	m_graphicsPipeline.Destroy();

	m_basicShader.Destroy();

	m_LeVkCore.FreeCommandBuffers(LE_MAX_FRAMES_IN_FLIGHT, m_cmdBufs.data());

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
	m_pSwapchain = m_LeVkCore.GetSwapchain();
	m_basicShader.Init(m_LeVkCore.GetDevice(), TESTAPP_SHADER_BIN_PATH "basic.spv");

	m_graphicsPipeline.Init(m_LeVkCore.GetDevice(), m_LeVkCore.GetSwapchain(), &m_basicShader);

	CreateCommandBuffers();

	m_frameIndex = 0;

}

void TestAppWindow::Draw()
{

	m_pRenderQueue->WaitForFences(m_frameIndex);
	m_pRenderQueue->ResetFences(m_frameIndex);

	uint32 ImageIndex = m_pRenderQueue->AcquireNextImage(m_frameIndex);
	RecordCommandBuffer(m_cmdBufs[m_frameIndex], ImageIndex);



	m_pRenderQueue->SubmitAsync(m_cmdBufs[m_frameIndex], ImageIndex, m_frameIndex);



	m_pRenderQueue->Present(ImageIndex);


	m_frameIndex = (m_frameIndex + 1) % LE_MAX_FRAMES_IN_FLIGHT;
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
	m_cmdBufs.resize(LE_MAX_FRAMES_IN_FLIGHT);
	m_LeVkCore.CreateCommandBuffers(LE_MAX_FRAMES_IN_FLIGHT, m_cmdBufs.data());
}

void TestAppWindow::RecordCommandBuffer(VkCommandBuffer cmdBuf, uint32 image_index)
{

	VkClearValue ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	LE_DEBUG_ASSERT(int32(image_index) < m_numImages);

	m_LeVkCore.BeginCommandBuffer(cmdBuf, 0);

	// setup image barrier for clearing the color

	TransitionImageLayout(cmdBuf, m_pSwapchain->GetImage(image_index),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);


	VkExtent2D swapchainExtent = m_pSwapchain->GetExtent();

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = m_pSwapchain->GetImageView(image_index);
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue = ClearColor;

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = swapchainExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;

	vkCmdBeginRendering(cmdBuf, &renderingInfo);

	m_graphicsPipeline.Bind(cmdBuf);

	VkViewport Viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)swapchainExtent.width,
		.height = (float)swapchainExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D Scissor = 
	{
		.offset =
		{
			.x = 0,
			.y = 0,
		},
		.extent =
		{
			.width = (uint32)swapchainExtent.width,
			.height = (uint32)swapchainExtent.height
		}
	};


	vkCmdSetViewport(cmdBuf, 0, 1, &Viewport);
	vkCmdSetScissor(cmdBuf, 0, 1, &Scissor);

	vkCmdDraw(cmdBuf, 3, 1, 0, 0);

	vkCmdEndRendering(cmdBuf);

		
	// transition the image for presenting

	TransitionImageLayout(cmdBuf, m_pSwapchain->GetImage(image_index),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

	m_LeVkCore.EndCommandBuffer(cmdBuf);

}


// helper funcs for recording command buffers
void TestAppWindow::TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
	VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask)
{

	VkImageMemoryBarrier2 barrier
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessMask,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};


	VkDependencyInfo DependencyInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.dependencyFlags = 0,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
	};

	vkCmdPipelineBarrier2(cmdBuf, &DependencyInfo);
}