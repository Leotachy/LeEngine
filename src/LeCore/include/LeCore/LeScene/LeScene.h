

#pragma once

#include "LeCore/LeTexture/LeTextureDictionary.h"
#include "LeCore/LeMaterial/LeMaterialDictionary.h"
#include "LeCore/LeShader/LeShaderDictionary.h"
#include "LeCore/LeWorld/LeWorld.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeScene : public LeBinaryChunk
{
public:
	LeScene();
	virtual ~LeScene() = default;

	LeTextureDictionary* GetTextureDictionary(void) { return &m_textureDictionary; }
	LeMaterialDictionary* GetMaterialDictionary(void) { return &m_materialDictionary; }
	LeWorld* GetWorld(void) { return &m_world; }

protected:

	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

private:

	LeTextureDictionary m_textureDictionary;
	LeMaterialDictionary m_materialDictionary;
	LeShaderDictionary m_shaderDictionary;
	LeWorld m_world;
};