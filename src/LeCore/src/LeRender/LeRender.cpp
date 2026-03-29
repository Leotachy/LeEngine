
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

LeRenderApi LeRenderInterface::LeGetRenderApi(void)
{
	return RenderApi;
}

void LeRenderInterface::LeInitRenderer(InitRendererParams& params)
{

	RenderApi = params.api;

	switch (params.api)
	{
	case LeRenderApi::LE_RENDER_API_NONE:
		LE_ASSERT(0);
		break;

	case LeRenderApi::LE_RENDER_API_VULKAN:
		LeVkCore::Singleton().Init(params.pAppName, params.pWindow);

		BeginRenderFunc = LeVkBeginRender;
		RenderFunc = LeVkRender;
		EndRenderFunc = LeVkEndRender;

		break;
	}
}

void LeRenderInterface::LeShutdownRenderer()
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

void LeRenderInterface::LeDeInitRenderer(void)
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

LeBool LeRenderInterface::LeBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	return BeginRenderFunc(pCam, bFramebufferResized);
}

LeBool LeRenderInterface::LeRender(LeMesh* pMesh)
{
	return RenderFunc(pMesh);
}

LeBool LeRenderInterface::LeEndRender(void)
{
	return EndRenderFunc();
}

LeBool LeRenderInterface::LeVkBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	return LeVkCore::Singleton().LeVkBeginRender(pCam, bFramebufferResized);
}

LeBool LeRenderInterface::LeVkRender(LeMesh* pMesh)
{
	return LeVkCore::Singleton().LeVkRender(pMesh);
}

LeBool LeRenderInterface::LeVkEndRender(void)
{
	return LeVkCore::Singleton().LeVkEndRender();
}

LeRenderApi LeRenderInterface::RenderApi = LeRenderApi::LE_RENDER_API_NONE;

LeBeginRenderFunc LeRenderInterface::BeginRenderFunc = NULL;
LeRenderFunc LeRenderInterface::RenderFunc = NULL;
LeEndRenderFunc LeRenderInterface::EndRenderFunc = NULL;