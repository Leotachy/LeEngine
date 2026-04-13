

#pragma once

#include "LeCore/LeBinary/LeBinaryChunk.h"

#include "LeCore/LeTexture/LeTextureDictionary.h"
#include "LeCore/LeMaterial/LeMaterialDictionary.h"
#include "LeCore/LeWorld/LeWorld.h"

class LeScene : public LeBinaryChunk
{
public:
	LeScene();
	virtual ~LeScene() = default;

protected:

	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

private:

	LeTextureDictionary m_textureDictionary;
	LeMaterialDictionary m_materialDictionary;
	LeWorld m_world;
};