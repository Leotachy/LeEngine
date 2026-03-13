
#pramga once

// Lukas Andrijauskas
// KTU Programø sistemos baigiamasis darbas
// 2026


#include "LeCore/LeRender/LeRender.h"

struct InitEngineParams
{
	InitRendererParams RendererParams;
};

void LeEngineInit(InitEngineParams& Params);
void LeEngineTerm(void);