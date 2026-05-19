
#include "LeCore/lecore.h"

#include "LeCore/LeMaterial/LeMaterialDictionary.h"

LeMaterialDictionary::~LeMaterialDictionary()
{
	PurgeAllMaterials();
}

LeMaterial* LeMaterialDictionary::FindMaterial(uint64 materialHash)
{
	LE_DEBUG_ASSERT(materialHash);

	auto found = m_materials.find(materialHash);
	if (found != m_materials.end())
	{
		return found->second;
	}

	return NULL;
}

LeBool LeMaterialDictionary::AddMaterial(LeMaterial* pMaterial)
{
	LE_DEBUG_ASSERT(pMaterial);

	uint64 hash = pMaterial->GetAttributeHash();

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

void LeMaterialDictionary::SerializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	uint32 numMaterials = (uint32)m_materials.size();
	pFileStream->WriteNumber(&numMaterials);
	for (auto pair : m_materials)
	{
		LeMaterial* pMaterial = pair.second;
		pMaterial->SerializeBinaryChunk(pFileStream);
	}
}

void LeMaterialDictionary::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::DeserializeBinaryChunk(pFileStream);

	uint32 numMaterials;
	pFileStream->ReadNumber(&numMaterials);
	for (uint32 i = 0; i < numMaterials; i++)
	{
		LeMaterial* pMaterial = LeMaterial::Create();
		pMaterial->DeserializeBinaryChunk(pFileStream);

		if (!AddMaterial(pMaterial))
			delete pMaterial;
	}
}
