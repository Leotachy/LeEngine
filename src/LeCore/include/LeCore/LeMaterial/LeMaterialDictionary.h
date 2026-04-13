
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"

class LeMaterialDictionary
{
public:

	LeMaterialDictionary() = default;
	virtual ~LeMaterialDictionary();

	LeMaterial* FindMaterial(uint64 attributeHash);
	LeBool AddMaterial(LeMaterial* pMaterial);
	void PurgeAllMaterials(void);

protected:

	std::unordered_map<uint64, LeMaterial*> m_materials;
};