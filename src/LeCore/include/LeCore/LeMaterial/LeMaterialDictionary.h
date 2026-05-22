
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

class LeMaterialDictionary : public LeSerializableBinaryChunk
{
public:

	virtual ~LeMaterialDictionary();

	LeMaterial* FindMaterial(uint64 materialHash);
	LeBool AddMaterial(LeMaterial* pMaterial);
	void PurgeAllMaterials(void);

	template<typename Func>
	void ForEachMaterial(Func&& func)
	{
		for (auto pair : m_materials)
		{
			LeMaterial* pMaterial = pair.second;
			func(pMaterial);
		}
	}

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::MaterialDictionary; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

protected:

	std::unordered_map<uint64, LeMaterial*> m_materials;
};