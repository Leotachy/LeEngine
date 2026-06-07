
#pragma once

#include <GLFW/glfw3.h>

#include "LeCore/LeCore.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeCamera/LeCamera.h"
#include "LeCore/LeScene/LeScene.h"
#include "LeCore/LeShader/LeShaderDictionary.h"

// Lukas Andrijauskas
// 11th March, 2026

enum class LeRenderApi
{
	None,
	Vulkan,
};

struct WindowParams
{
	GLFWwindow* pWindow;
	const char* pAppName;
};

struct ViewportParams
{
	uint32 ViewportWidth, ViewportHeight;
	LeBool bRenderViewportToImGuiTexture;
};

struct PathParams
{
	const LeChar* pShaderBinaryPath;
};

struct InitRendererParams
{
	LeRenderApi API;

	WindowParams WindowParams;
	ViewportParams ViewportParams;
	PathParams PathParams;
};

typedef LeBool(*LeRenderFunc)(LeScene*, LeCamera*, LeBool);

class LeRenderInterface
{
public:
	static LeRenderApi GetRenderApi(void);

	static void InitRenderer(InitRendererParams* params);
	static void ShutdownRenderer();
	static void DeInitRenderer(void);

	static void SetViewportDimensions(uint32 width, uint32 height);
#ifdef SUPPORT_IMGUI
	static ImTextureID GetViewportImGuiTexture(void);
#endif

	static LeBool RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

	static const LeShaderDictionary* GetShaderDictionary(void);

private:

	static LeBool VkRenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized);

	static LeRenderApi RenderApi;

	static LeRenderFunc RenderFunc;
};