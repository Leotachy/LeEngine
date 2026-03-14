
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"

static LeRenderApi s_RenderApi = LeRenderApi::None;

LeRenderApi LeGetRenderApi(void)
{
	return s_RenderApi;
}

typedef LeBool (*LeBeginRenderFunc)(LeCamera*, LeBool);
LeBeginRenderFunc s_beginRenderFunc = NULL;

typedef LeBool (*LeRenderFunc)(LeMesh*);
LeRenderFunc s_renderFunc = NULL;

typedef LeBool (*LeEndRenderFunc)(void);
LeEndRenderFunc s_endRenderFunc = NULL;


void LeInitRenderer(InitRendererParams& params)
{

	switch (params.api)
	{
	case LeRenderApi::None:
		LE_ASSERT(0);
		break;

	case LeRenderApi::Vulkan:
		LeInitVulkan(params.pAppName, params.pWindow);

		s_beginRenderFunc = LeVkBeginRender;
		s_renderFunc = LeVkRender;
		s_endRenderFunc = LeVkEndRender;

		break;
	}

	s_RenderApi = params.api;
}

void LeShutdownRenderer()
{
	switch (s_RenderApi)
	{
	case LeRenderApi::None:
		break;

	case LeRenderApi::Vulkan:
		LeShutDownVulkan();
		break;
	}
}

void LeDeInitRenderer(void)
{
	switch (s_RenderApi)
	{
	case LeRenderApi::None:
		break;

	case LeRenderApi::Vulkan:
		LeDeInitVulkan();
		break;
	}

	s_RenderApi = LeRenderApi::None;
}


LeBool LeBeginRender(LeCamera* pCam, LeBool bFramebufferResized)
{
	return s_beginRenderFunc(pCam, bFramebufferResized);
}

LeBool LeRender(LeMesh* pMesh)
{
	return s_renderFunc(pMesh);
}

LeBool LeEndRender(void)
{
	return s_endRenderFunc();
}
