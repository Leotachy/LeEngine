

#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LeEditor/LeViewport.h"

class LeEditorInstance
{
public:
	LeEditorInstance(void);
	~LeEditorInstance(void);

	void Run(void);

private:

	void Update(void);

	void MakeDockWindows(void);
	ImGuiID MakeDockSpaceOverViewport(void);


	GLFWwindow* m_gltfWindow;

	LeBool m_bDocksBuilt;

	LeViewport m_Viewport;

};