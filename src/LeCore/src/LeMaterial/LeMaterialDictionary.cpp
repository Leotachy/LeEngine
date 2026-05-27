
#include "LeCore/lecore.h"

#include "LeCore/LeMaterial/LeMaterialDictionary.h"

LeMaterialDictionary::~LeMaterialDictionary()
{
	PurgeAllMaterials();
}

LeMaterial* LeMaterialDictionary::FindMaterial(uint64 materialHash)
{
	LE_DEBUG_ASSERT(materialHash);

	auto found = m_Materials.find(materialHash);
	if (found != m_Materials.end())
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

	m_Materials[hash] = pMaterial;
	return true;
}

void LeMaterialDictionary::PurgeAllMaterials(void)
{
	for (auto pair : m_Materials)
	{
		LeMaterial* pMaterial = pair.second;
		delete pMaterial;
	}

	m_Materials.clear();
}

void LeMaterialDictionary::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMaterials = (uint32)m_Materials.size();
	pFileStream->WriteNumber(&numMaterials);
	for (auto pair : m_Materials)
	{
		LeMaterial* pMaterial = pair.second;
		pMaterial->SerializeBinaryChunk(pFileStream, this);
	}
}

void LeMaterialDictionary::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMaterials;
	pFileStream->ReadNumber(&numMaterials);
	for (uint32 i = 0; i < numMaterials; i++)
	{
		LeMaterial* pMaterial = LeMaterial::Create();
		pMaterial->DeserializeBinaryChunk(pFileStream, this);

		if (!AddMaterial(pMaterial))
			delete pMaterial;
	}
}
