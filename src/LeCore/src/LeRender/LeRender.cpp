
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
		LE_ASSERT(0);
		break;

	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().Init(params.pAppName, params.pWindow);

		BeginRenderFunc = VkBeginRender;
		RenderFunc = VkRender;
		EndRenderFunc = VkEndRender;

		break;
	}
}

void LeRenderInterface::ShutdownRenderer()
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		break;

	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().ShutDown();
		break;
	}
}

void LeRenderInterface::InitRenderer(void)
{
	switch (RenderApi)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		break;

	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().DeInit();

		BeginRenderFunc = NULL;
		RenderFunc = NULL;
		EndRenderFunc = NULL;

		break;
	}

	RenderApi = LeRenderApi::LE_RENDER_API_NONE;
}

LeBool LeRenderInterface::BeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	return BeginRenderFunc(pCam, bFramebufferResized);
}

LeBool LeRenderInterface::Render(LeMesh* pMesh)
{
	return RenderFunc(pMesh);
}

LeBool LeRenderInterface::EndRender(void)
{
	return EndRenderFunc();
}

LeBool LeRenderInterface::VkBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	return LeVkCore::Singleton().LeVkBeginRender(pCam, bFramebufferResized);
}

LeBool LeRenderInterface::VkRender(LeMesh* pMesh)
{
	return LeVkCore::Singleton().LeVkRender(pMesh);
}

LeBool LeRenderInterface::VkEndRender(void)
{
	return LeVkCore::Singleton().LeVkEndRender();
}

LeRenderApi LeRenderInterface::RenderApi = LeRenderApi::LE_RENDER_API_NONE;

LeBeginRenderFunc LeRenderInterface::BeginRenderFunc = NULL;
LeRenderFunc LeRenderInterface::RenderFunc = NULL;
LeEndRenderFunc LeRenderInterface::EndRenderFunc = NULL;