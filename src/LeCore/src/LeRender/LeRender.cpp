
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

static LeRenderApi s_RenderApi = LeRenderApi::None;

LeRenderApi LeGetRenderApi(void)
{
	return s_RenderApi;
}

void LeInitRenderer(InitRendererParams& params)
{

	switch (params.api)
	{
	case LeRenderApi::None:
		LE_ASSERT(0);
		break;

	case LeRenderApi::Vulkan:
		LeVkInit(params.pAppName, params.pWindow);
		break;
	}

	s_RenderApi = params.api;
}

void LeTermRenderer(void)
{
	switch (s_RenderApi)
	{
	case LeRenderApi::None:
		break;

	case LeRenderApi::Vulkan:
		LeVkTerminate();
		break;
	}

	s_RenderApi = LeRenderApi::None;
}


LeBool LeBeginRender(LeCamera* pCam)
{

}

void LeRender(LeMesh* pMesh)
{

}

void LeEndRender(void)
{

}
