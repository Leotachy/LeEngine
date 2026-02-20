
#pragma once

#include "lecore/lecore.h"

#include "LeWindow.h"
#include "LePipeline.h"
#include "LeDevice.h"


class FirstApp
{

public:

	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void Run();

private:

	LeWindow m_LeWindow{ WIDTH, HEIGHT, "LeEngine Vulkan Window" };
	LeDevice m_LeDevice{ m_LeWindow };
	LePipeline m_LePipeline
	{ 
		LePipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT),
		m_LeDevice,
		LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.vert.spv",
		LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.frag.spv" 
	};
};
