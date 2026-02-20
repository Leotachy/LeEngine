
#pragma once

#include "lecore/lecore.h"

#include "LeDevice.h"
#include "LePipeline.h"
#include "LeSwapChain.h"
#include "LeWindow.h"


class FirstApp
{

public:

	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	FirstApp();
	~FirstApp();

	FirstApp(const FirstApp&) = delete;
	FirstApp& operator=(const FirstApp&) = delete;

	void Run();

private:

	void CreatePipelineLayout();
	void CreatePipeline();
	void CreateCommandBuffers();
	void DrawFrame();

	LeWindow m_LeWindow{ WIDTH, HEIGHT, "LeEngine Vulkan Window" };
	LeDevice m_LeDevice{ m_LeWindow };
	LeSwapChain m_LeSwapChain{ m_LeDevice, m_LeWindow.getExtent() };
	LePipeline m_LePipeline;
	
	VkPipelineLayout m_PipelineLayout;
	std::vector<VkCommandBuffer> m_CommandBuffers;
};
