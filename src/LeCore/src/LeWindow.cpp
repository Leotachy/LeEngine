
#include "lecore/lecore.h"
#include "LeWindow.h"



LeWindow::LeWindow(int width, int height, const char* name) : m_width{ width }, m_height{ height }, m_name{ name }
{
	Init();
}

LeWindow::~LeWindow(void)
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void LeWindow::Init(void)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(m_width, m_height, m_name, nullptr, nullptr);

}

bool LeWindow::ShouldClose(void)
{
	return glfwWindowShouldClose(m_window);
}

void LeWindow::PollEvents(void)
{
	glfwPollEvents();
}


void LeWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
	{
		LE_ERROR("CreateWindowSurface: Failed to create glfwWindowSurface");
		return;
	}


}
