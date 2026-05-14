
#pragma once

#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeScene/LeScene.h"

// Lukas Andrijauskas
// 11th March, 2026

enum LeRenderApi
{
	LE_RENDER_API_NONE,
	LE_RENDER_API_VULKAN,
};

struct WindowParams
{
	GLFWwindow* pWindow;
	const char* pAppName;
};

struct ViewportParams
{
	uint32 uViewportWidth, uViewportHeight;
	LeBool bRenderViewportToImGuiTexture;
};

struct PathParams
{
	const LeChar* pShaderBinaryPath;
};

struct InitRendererParams
{
	LeRenderApi api;

	WindowParams windowParams;
	ViewportParams viewportParams;
	PathParams pathParams;
};

typedef LeBool(*LeRenderFunc)(LeScene*, LeCamera*, LeBool);

class LeRenderInterface
{
public:
	static LeRenderApi GetRenderApi(void);

	static void InitRenderer(InitRendererParams& params);
	static void ShutdownRenderer();
	static void DeInitRenderer(void);

	static void SetViewportDimensions(uint32 width, uint32 height);

#ifdef SUPPORT_IMGUI
	static ImTextureID GetViewportImGuiTexture(void);
#endif

	static LeBool RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

private:

	static LeBool VkRenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

	static LeRenderApi RenderApi;

	static LeRenderFunc RenderFunc;
};