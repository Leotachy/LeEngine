
#include "FirstApp.h"


FirstApp::FirstApp()
{
	CreatePipelineLayout();
	CreatePipeline();
	CreateCommandBuffers();
}

FirstApp::~FirstApp()
{
	vkDestroyPipelineLayout(m_LeDevice.device(), m_PipelineLayout, nullptr);
	m_LePipeline.DeInit();

}

void FirstApp::Run()
{

	while (!m_LeWindow.ShouldClose())
	{
		m_LeWindow.PollEvents();

		DrawFrame();

	}

	vkDeviceWaitIdle(m_LeDevice.device());
}

void FirstApp::CreatePipelineLayout()
{

	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = 0;
	PipelineLayoutInfo.pSetLayouts = nullptr;
	PipelineLayoutInfo.pushConstantRangeCount = 0;
	PipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_LeDevice.device(), &PipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		LE_ERROR("Failed to create pipeline layout");
	}

}

void FirstApp::CreatePipeline()
{
	LePipelineConfigInfo PipelineConfig{ LePipeline::DefaultPipelineConfigInfo(m_LeSwapChain.width(), m_LeSwapChain.height()) };
	PipelineConfig.renderPass = m_LeSwapChain.getRenderPass();
	PipelineConfig.pipelineLayout = m_PipelineLayout;
	
	m_LePipeline.Init(
		PipelineConfig,
		m_LeDevice,
		LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.vert.spv",
		LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.frag.spv"
	);

}

void FirstApp::CreateCommandBuffers()
{

	m_CommandBuffers.resize(m_LeSwapChain.imageCount());

	VkCommandBufferAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocInfo.commandPool = m_LeDevice.getCommandPool();
	AllocInfo.commandBufferCount = uint32(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_LeDevice.device(), &AllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		LE_ERROR("Failed to allocate command buffers");

	int32 i = 0;
	for (VkCommandBuffer& CommandBuffer : m_CommandBuffers)
	{
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
			LE_ERROR("Command buffer failed to begin recording");

		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = m_LeSwapChain.getRenderPass();
		RenderPassInfo.framebuffer = m_LeSwapChain.getFrameBuffer(i);

		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = m_LeSwapChain.getSwapChainExtent();

		VkClearValue ClearValues[2]{};
		ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		ClearValues[1].depthStencil = { 1.0f, 0 };

		RenderPassInfo.clearValueCount = ARRAY_SIZE(ClearValues);
		RenderPassInfo.pClearValues = ClearValues;

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_LePipeline.Bind(CommandBuffer);
		vkCmdDraw(CommandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(CommandBuffer);
		if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
			LE_ERROR("Command buffer failed to finish recording");


		i++;
	}
}

void FirstApp::DrawFrame()
{
	uint32 ImageIndex;
	VkResult result = m_LeSwapChain.acquireNextImage(&ImageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		LE_ERROR("Failed to acquire image through swap chain");

	result = m_LeSwapChain.submitCommandBuffers(&m_CommandBuffers[ImageIndex], &ImageIndex);
	if (result != VK_SUCCESS)
		LE_ERROR("Failed to submit command buffers");



}


