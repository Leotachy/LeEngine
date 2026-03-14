
#include "LeCore/LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"
#include "LeCore/LeRender/LeRender.h"

void LeEngineInit(InitEngineParams& Params)
{
	LeInitRenderer(Params.RendererParams);
}

void LeEngineShutDown(void)
{
	LeShutdownRenderer();
}

void LeEngineDeInit(void)
{
	LeDeInitRenderer();
}