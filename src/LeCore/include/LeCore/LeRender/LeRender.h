
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
	static LeRenderApi LeGetRenderApi(void);

	static void LeInitRenderer(InitRendererParams& params);
	static void LeShutdownRenderer();
	static void LeDeInitRenderer(void);

	static LeBool LeBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
	static LeBool LeRender(LeMesh* pMesh);
	static LeBool LeEndRender(void);

private:

	static LeBool LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized);
	static LeBool LeVkRender(LeMesh* pMesh);
	static LeBool LeVkEndRender(void);

	static LeRenderApi RenderApi;

	static LeBeginRenderFunc BeginRenderFunc;
	static LeRenderFunc RenderFunc;
	static LeEndRenderFunc EndRenderFunc;
};