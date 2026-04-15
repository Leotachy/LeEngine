
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

void LeMaterialDictionary::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMaterials = (uint32)m_materials.size();
	pFileStream->WriteNumber(&numMaterials);
	for (auto pair : m_materials)
	{
		LeMaterial* pMaterial = pair.second;

		pFileStream->WriteString(pMaterial->m_name);

		pFileStream->Write(&pMaterial->attribs, sizeof(pMaterial->attribs));
	}
}

void LeMaterialDictionary::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMaterials;
	pFileStream->ReadNumber(&numMaterials);
	for (uint32 i = 0; i < numMaterials; i++)
	{
		LeMaterial* pMaterial = LeMaterial::Create();

		pFileStream->ReadString(pMaterial->m_name, LE_MATERIAL_MAX_NAME);

		pFileStream->Read(&pMaterial->attribs, sizeof(pMaterial->attribs));
	}
}
