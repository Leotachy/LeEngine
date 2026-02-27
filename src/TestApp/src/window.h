


#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeVulkanQueue.h"

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
	
	LeShader m_basicShader;

public:

	void Draw();

	VkExtent2D getExtent() { return { uint32(m_width), uint32(m_height) }; }

	bool ShouldClose(void);
	void PollEvents(void);

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);


private:

	void CreateCommandBuffers(void);
	void RecordCommandBuffers(void);

};

