
#include "LeCore/lecore.h"

#include "LeCore/LeMaterial/LeMaterialDictionary.h"

LeMaterialDictionary::~LeMaterialDictionary()
{
	PurgeAllMaterials();
}

LeMaterial* LeMaterialDictionary::FindMaterial(uint64 attributeHash)
{
	LE_DEBUG_ASSERT(attributeHash);

	auto found = m_materials.find(attributeHash);
	if (found != m_materials.end())
	{
		return found->second;
	}

	return NULL;
}

LeBool LeMaterialDictionary::AddMaterial(LeMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(pMaterial);

	uint64 hash = pMaterial->CalculateAttributeHash();

	if (FindMaterial(hash))
	{
		return false;
	}

	m_materials[hash] = pMaterial;
	return true;
}

void LeMaterialDictionary::PurgeAllMaterials(void)
{
	for (auto pair : m_materials)
	{
		LeMaterial* pMaterial = pair.second;
		delete pMaterial;
	}

	m_materials.clear();
}

