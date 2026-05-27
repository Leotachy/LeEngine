
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

LeRenderApi LeRenderInterface::GetRenderApi(void)
{
	return RenderApi;
}

void LeRenderInterface::InitRenderer(InitRendererParams* params)
{

	RenderApi = params->API;

	switch (params->API)
	{
	case LeRenderApi::None:
		LE_ERROR("Bad render API: %u", RenderApi);
		break;
	case LeRenderApi::Vulkan:
		LeVkRenderer::Get()->Init(&params->WindowParams, &params->ViewportParams, &params->PathParams);
		RenderFunc = VkRenderScene;
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		break;
	}
}

void LeRenderInterface::ShutdownRenderer()
{
	switch (RenderApi)
	{
	case LeRenderApi::None:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::Vulkan:
		LeVkRenderer::Get()->ShutDown();
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		break;
	}
}

void LeRenderInterface::DeInitRenderer(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::None:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::Vulkan:
		LeVkRenderer::Get()->DeInit();
		RenderFunc = NULL;
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		break;
	}

	RenderApi = LeRenderApi::None;
}

void LeRenderInterface::SetViewportDimensions(uint32 width, uint32 height)
{
	switch (RenderApi)
	{
	case LeRenderApi::None:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::Vulkan:
		LeVkRenderer::Get()->SetViewportDimensions(width, height);
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		break;
	}

}

#ifdef SUPPORT_IMGUI

ImTextureID LeRenderInterface::GetViewportImGuiTexture(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::None:
		LE_ERROR("Uninitialized renderer");
		return 0;
		break;
	case LeRenderApi::Vulkan:
		return LeVkRenderer::Get()->GetViewportImGuiTexture();
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		return 0;
		break;
	}
}

#endif

LeBool LeRenderInterface::RenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	return RenderFunc(pScene, pCam, bFramebufferResized);
}

LeBool LeRenderInterface::VkRenderScene(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	return LeVkRenderer::Get()->RenderScene(pScene, pCam, bFramebufferResized);
}

const LeShaderDictionary* LeRenderInterface::GetShaderDictionary(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::None:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::Vulkan:
		return LeVkRenderer::Get()->GetShaderDictionary();
		break;
	default:
		LE_ERROR("Unknown render API: %u", RenderApi);
		break;
	}

	return NULL;
}

LeRenderApi LeRenderInterface::RenderApi = LeRenderApi::None;

LeRenderFunc LeRenderInterface::RenderFunc = NULL;