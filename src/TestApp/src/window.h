


#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeVulkanQueue.h"
#include "LeCore/LeRender/LeGraphicsPipeline.h"

class TestAppWindow
{
public:

	TestAppWindow(int width, int height, const char* name);
	~TestAppWindow(void);

	TestAppWindow(const TestAppWindow&) = delete;
	TestAppWindow& operator=(const TestAppWindow&) = delete;

private:

	void Init(void);

	const int m_width;
	const int m_height;
	const char* m_name;

	GLFWwindow* m_window;

	LeVulkanCore m_LeVkCore;
	int32 m_numImages = 0;
	std::vector<VkCommandBuffer> m_cmdBufs;
	LeVulkanQueue* m_pRenderQueue = NULL;
	LeSwapchain* m_pSwapchain = NULL;

	LeShader m_basicShader;

	LeGraphicsPipeline m_graphicsPipeline;

	int32 m_frameIndex;

public:

	void Draw();
	void WaitIdle(void) { m_pRenderQueue->WaitIdle(); }

	VkExtent2D getExtent() { return { uint32(m_width), uint32(m_height) }; }

	bool ShouldClose(void);
	void PollEvents(void);

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);


private:

	void CreateCommandBuffers(void);
	void RecordCommandBuffer(VkCommandBuffer cmdBuf, uint32 image_index);

	// helper funcs for recording command buffers
	void TransitionImageLayout(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
		VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
		VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlagBits2 dstStageMask);


};

