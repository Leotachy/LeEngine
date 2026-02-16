
#pragma once

#include "LeWindow.h"
#include "LePipeline.h"

namespace Le
{
	class FirstApp
	{

	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void Run();

	private:

		LeWindow m_LeWindow{WIDTH, HEIGHT, "LeEngine Vulkan Window"};
		LePipeline m_LePipeline{ LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.vert.spv",
								 LECORE_RESOURCES_PATH "shaders/compiled/basic_vk.frag.spv" };
	};
}