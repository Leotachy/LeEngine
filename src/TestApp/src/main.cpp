

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeGraphicsPipeline.h"
#include "LeCore/LeMesh/LeSimpleMesh.h"
#include "LeCore/LeCamera/LeCamera.h"


#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

static GLFWwindow* pWindow;

static LeVulkanCore LeVkCore;
static int32 NumImages = 0;
static std::vector<VkCommandBuffer> CommandBuffers;
static LeVulkanQueue* pRenderQueue = NULL;
static LeSwapchain* pSwapchain = NULL;

static LeShader BasicShader;

static LeGraphicsPipeline TheGraphicsPipeline;

static int32 iFrameIdx;

static LeBool bFrameBufferResized;

static LeSimpleMesh TestMesh;

std::vector<LeMemoryBuffer> m_uniformBuffers;

static LeCamera* pCam = NULL;


static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	bFrameBufferResized = true;

	if (pCam)
	{
		pCam->SetScreenSize(width, height);
	}
}

static void processInput(GLFWwindow* window, float deltaT)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::FORWARD, deltaT);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::BACKWARD, deltaT);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::LEFT, deltaT);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pCam->ProcessKeyboard(CamMovement::RIGHT, deltaT);
}


static bool s_lastInit = false;
static float s_lastX = 0.0f;
static float s_lastY = 0.0f;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (s_lastInit)
	{
		float xoffset = (float)xpos - s_lastX;
		float yoffset = ((float)ypos - s_lastY);

		pCam->ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		s_lastInit = true;
	}

	s_lastX = (float)xpos;
	s_lastY = (float)ypos;

}

static const LeVertex Vertices[] =
{
	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
};
static const int32 iNumVertices = ARRAY_SIZE(Vertices);


static void updateUniformBuffers(uint32_t ImageIndex)
{
	static float foo = 0.0f;
	glm::mat4 Rotate = glm::mat4(1.0);
	Rotate = glm::rotate(Rotate, glm::radians(foo), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
	foo += 0.001f;

	const glm::mat4* ViewProj = pCam->GetViewProjMatrix();

	m_uniformBuffers[ImageIndex].Update(LeVkCore.GetDevice(), ViewProj, sizeof(glm::mat4));
}

static void createVertexBuffer()
{



	TestMesh.m_vertexBufferSize = sizeof(Vertices[0]) * iNumVertices;
	LeVkCore.CreateVertexBuffer(Vertices, TestMesh.m_vertexBufferSize, TestMesh.m_vb);
}

struct UniformData
{
	glm::mat4 mvp;
};

static void createUniformBuffers()
{
	LeVkCore.CreateUniformBuffers(sizeof(UniformData), m_uniformBuffers);
}

static void testAppInit(void)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	pWindow = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "TestApp", nullptr, nullptr);

	pCam = new LeCamera(PROJECTION_TYPE_PERSPECTIVE, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glm::vec3 start = glm::vec3(0.0f, 0.0f, 3.0f);
	pCam->SetPosition(&start);

	glfwSetCursorPosCallback(pWindow, mouse_callback);

	LeVkCore.Init("TestApp", pWindow);
	NumImages = LeVkCore.GetSwapchain()->GetNumImages();
	pRenderQueue = LeVkCore.GetQueue();
	pSwapchain = LeVkCore.GetSwapchain();
	BasicShader.Init(LeVkCore.GetDevice(), TESTAPP_SHADER_BIN_PATH "basic.spv");

	createVertexBuffer();
	createUniformBuffers();

	TheGraphicsPipeline.Init(LeVkCore.GetDevice(), LeVkCore.GetSwapchain(), &BasicShader, &TestMesh, NumImages,
		m_uniformBuffers, sizeof(UniformData));

	CommandBuffers.resize(LE_MAX_FRAMES_IN_FLIGHT);
	LeVkCore.CreateCommandBuffers(LE_MAX_FRAMES_IN_FLIGHT, CommandBuffers.data());

	iFrameIdx = 0;

	bFrameBufferResized = false;
}

static void testAppCleanup(void)
{


	LeVkCore.FreeCommandBuffers(LE_MAX_FRAMES_IN_FLIGHT, CommandBuffers.data());

	TheGraphicsPipeline.Destroy();

	for (int i = 0; i < m_uniformBuffers.size(); i++)
		m_uniformBuffers[i].Destroy(LeVkCore.GetDevice());

	TestMesh.Destroy(LeVkCore.GetDevice());

	BasicShader.Destroy();

	delete pCam;
	pCam = NULL;

	glfwDestroyWindow(pWindow);
	glfwTerminate();
}


static void transitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
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

static void recordCommandBuffer(VkCommandBuffer cmdBuf, uint32 image_index)
{

	VkClearValue ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	LE_DEBUG_ASSERT(int32(image_index) < NumImages);

	LeVkCore.BeginCommandBuffer(cmdBuf, 0);

	// setup image barrier for clearing the color

	transitionImageLayout(cmdBuf, pSwapchain->GetImage(image_index),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);


	VkExtent2D swapchainExtent = pSwapchain->GetExtent();

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = pSwapchain->GetImageView(image_index);
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

	TheGraphicsPipeline.Bind(cmdBuf, image_index);

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

	transitionImageLayout(cmdBuf, pSwapchain->GetImage(image_index),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

	LeVkCore.EndCommandBuffer(cmdBuf);

}



static void draw()
{

	pRenderQueue->WaitForFences(iFrameIdx);


	uint32 ImageIndex = pRenderQueue->AcquireNextImage(iFrameIdx);

	if (ImageIndex == -1)
	{
		LeVkCore.RecreateSwapChain();
		return;
	}

	pRenderQueue->ResetFences(iFrameIdx);

	updateUniformBuffers(ImageIndex);

	recordCommandBuffer(CommandBuffers[iFrameIdx], ImageIndex);



	pRenderQueue->SubmitAsync(CommandBuffers[iFrameIdx], ImageIndex, iFrameIdx);



	if (!pRenderQueue->Present(ImageIndex) || bFrameBufferResized)
	{
		bFrameBufferResized = false;
		LeVkCore.RecreateSwapChain();
	}


	iFrameIdx = (iFrameIdx + 1) % LE_MAX_FRAMES_IN_FLIGHT;
}

int main(void)
{
	testAppInit();

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(pWindow, deltaTime);
		draw();
	}

	pRenderQueue->WaitIdle();

	testAppCleanup();

}


