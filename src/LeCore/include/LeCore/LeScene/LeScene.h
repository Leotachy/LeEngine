

#pragma once

#include "LeCore/LeTexture/LeTextureDictionary.h"
#include "LeCore/LeMaterial/LeMaterialDictionary.h"
#include "LeCore/LeWorld/LeWorld.h"

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

class LeScene : public LeSerializableBinaryChunk
{
public:
	LeScene();
	virtual ~LeScene() = default;

	LeTextureDictionary* GetTextureDictionary(void) { return &m_TextureDictionary; }
	LeMaterialDictionary* GetMaterialDictionary(void) { return &m_MaterialDictionary; }
	LeWorld* GetWorld(void) { return &m_World; }

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::Scene; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

protected:



private:

	LeTextureDictionary m_TextureDictionary;
	LeMaterialDictionary m_MaterialDictionary;
	LeWorld m_World;
};