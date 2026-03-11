
#include "LeCore.h"
#include "LeCore/LeEngine/LeEngine.h"


void LeEngineInit(InitEngineParams& Params)
{
	LeInitRenderer(Params.RendererParams);
}

void LeEngineTerm(void)
{
	LeTermRenderer();
}