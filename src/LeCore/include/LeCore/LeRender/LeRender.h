
#pragma once

#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeCamera/LeCamera.h"

// Lukas Andrijauskas
// 11th March, 2026

enum LeRenderApi
{
	LE_RENDER_API_NONE,
	LE_RENDER_API_VULKAN,
};

struct InitRendererParams
{
	LeRenderApi api;
	GLFWwindow* pWindow;
	const char* pAppName;
};

typedef LeBool(*LeBeginRenderFunc)(LeCamera*, LeBool);
typedef LeBool(*LeRenderFunc)(LeMesh*);
typedef LeBool(*LeEndRenderFunc)(void);

class LeRenderInterface
{
public:
	static LeRenderApi GetRenderApi(void);

	static void InitRenderer(InitRendererParams& params);
	static void ShutdownRenderer();
	static void DeInitRenderer(void);

	static LeBool BeginRender(LeCamera* pCam, LeBool bFramebufferResized);
	static LeBool Render(LeMesh* pMesh);
	static LeBool EndRender(void);

private:

	static LeBool VkBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
	static LeBool VkRender(LeMesh* pMesh);
	static LeBool VkEndRender(void);

	static LeRenderApi RenderApi;

	static LeBeginRenderFunc BeginRenderFunc;
	static LeRenderFunc RenderFunc;
	static LeEndRenderFunc EndRenderFunc;
};