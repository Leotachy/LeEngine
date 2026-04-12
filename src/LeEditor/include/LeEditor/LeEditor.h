

#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class LeEditorInstance
{
public:
	LeEditorInstance(void);
	~LeEditorInstance(void);

	void Run(void);

private:

	GLFWwindow* m_gltfWindow;

};