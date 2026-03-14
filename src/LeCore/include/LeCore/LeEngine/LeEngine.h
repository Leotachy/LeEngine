
#pragma once

// Lukas Andrijauskas
// KTU Programu sistemos baigiamasis darbas
// 2026


#include "LeCore/LeRender/LeRender.h"

struct InitEngineParams
{
	InitRendererParams RendererParams;
};

void LeEngineInit(InitEngineParams& Params);
void LeEngineShutDown(void);
void LeEngineDeInit(void);