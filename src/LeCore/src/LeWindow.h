
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


class LeWindow
{
public:

	LeWindow(int width, int height, const char* name);
	~LeWindow(void);

	LeWindow(const LeWindow&) = delete;
	LeWindow& operator=(const LeWindow&) = delete;

private:

	void Init(void);

	const int m_width;
	const int m_height;
	const char* m_name;

	GLFWwindow* m_window;


public:

	bool ShouldClose(void);
	void PollEvents(void);

	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
};

