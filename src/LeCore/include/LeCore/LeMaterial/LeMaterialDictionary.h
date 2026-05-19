
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeMaterialDictionary : public LeBinaryChunk
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

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

protected:

	std::unordered_map<uint64, LeMaterial*> m_materials;
};