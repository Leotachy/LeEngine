

#pragma once

#include "LeCore/LeScene/LeScene.h"

class LebImporter
{
public:
	static LeScene* ImportGltf(const LeChar* pFilename);
};