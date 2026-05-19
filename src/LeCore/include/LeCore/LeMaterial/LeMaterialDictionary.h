
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeMaterialDictionary : public LeBinaryChunk
{
public:

	LeMaterialDictionary() = default;
	virtual ~LeMaterialDictionary();

	LeMaterial* FindMaterial(uint64 attributeHash);
	LeBool AddMaterial(LeMaterial* pMaterial);
	void PurgeAllMaterials(void);

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

protected:

	std::unordered_map<uint64, LeMaterial*> m_materials;
};