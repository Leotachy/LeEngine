
#pragma once

#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeCamera/LeCamera.h"

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
void LeShutdownRenderer();
void LeDeInitRenderer(void);



LeBool LeBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
LeBool LeRender(LeMesh* pMesh);
LeBool LeEndRender(void);
