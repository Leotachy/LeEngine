
#pragma once

#include <GLFW/glfw3.h>

// Lukas Andrijauskas
// 11th March, 2026

enum LeRenderApi
{
	None,
	Vulkan,
};

LeRenderApi LeGetRenderApi(void);

struct InitRendererParams
{
	LeRenderApi api;
	GLFWwindow* pWindow;
	const char* pAppName;
};

void LeInitRenderer(InitRendererParams& params);
void LeTermRenderer(void);
