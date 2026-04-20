
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

LeRenderApi LeRenderInterface::GetRenderApi(void)
{
	return RenderApi;
}

void LeRenderInterface::InitRenderer(InitRendererParams& params)
{

	RenderApi = params.api;

	switch (params.api)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ERROR("Bad render api: %u", RenderApi);
		break;
	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().Init(&params.windowParams, &params.viewportParams);
		RenderFunc = VkRender;
		break;
	default:
		LE_ERROR("Unknown render api: %u", RenderApi);
		break;
	}
}

void LeRenderInterface::ShutdownRenderer()
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().ShutDown();
		break;
	default:
		LE_ERROR("Unknown render api: %u", RenderApi);
		break;
	}
}

void LeRenderInterface::DeInitRenderer(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().DeInit();
		RenderFunc = NULL;
		break;
	default:
		LE_ERROR("Unknown render api: %u", RenderApi);
		break;
	}

	RenderApi = LeRenderApi::LE_RENDER_API_NONE;
}

void LeRenderInterface::SetViewportDimensions(uint32 width, uint32 height)
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ERROR("Uninitialized renderer");
		break;
	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().SetViewportDimensions(width, height);
		break;
	default:
		LE_ERROR("Unknown render api: %u", RenderApi);
		break;
	}

}

#ifdef SUPPORT_IMGUI

ImTextureID LeRenderInterface::GetViewportImGuiTexture(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ERROR("Uninitialized renderer");
		return 0;
		break;
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return LeVkCore::Singleton().GetViewportImGuiTexture();
		break;
	default:
		LE_ERROR("Unknown render api: %u", RenderApi);
		return 0;
		break;
	}
}

#endif

LeBool LeRenderInterface::Render(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	return RenderFunc(pScene, pCam, bFramebufferResized);
}

LeBool LeRenderInterface::VkRender(LeScene* pScene, LeCamera* pCam, LeBool bFramebufferResized)
{
	return LeVkCore::Singleton().Render(pScene, pCam, bFramebufferResized);
}

LeRenderApi LeRenderInterface::RenderApi = LeRenderApi::LE_RENDER_API_NONE;

LeRenderFunc LeRenderInterface::RenderFunc = NULL;