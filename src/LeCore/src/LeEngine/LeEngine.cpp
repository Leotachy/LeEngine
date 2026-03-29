
#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeRender/LeRender.h"

void LeEngineInit(InitEngineParams& Params)
{
	LeRenderInterface::LeInitRenderer(Params.RendererParams);
}

void LeEngineShutDown(void)
{
	LeRenderInterface::LeShutdownRenderer();
}

void LeEngineDeInit(void)
{
	LeRenderInterface::LeDeInitRenderer();
}