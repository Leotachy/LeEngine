

#pragma once

#include "LeCore/LeScene/LeScene.h"

class LebImporter
{
public:
	static LeBool ImportGltfToScene(const LeChar* pFilename, LeScene* pScene);
};