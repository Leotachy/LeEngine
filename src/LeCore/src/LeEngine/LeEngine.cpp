
#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeRender/LeRender.h"

void LeEngineInit(InitEngineParams* Params)
{
	LeRenderInterface::InitRenderer(&Params->RendererParams);
}

void LeEngineShutDown(void)
{
	LeRenderInterface::ShutdownRenderer();
}

void LeEngineDeInit(void)
{
	LeRenderInterface::DeInitRenderer();
}