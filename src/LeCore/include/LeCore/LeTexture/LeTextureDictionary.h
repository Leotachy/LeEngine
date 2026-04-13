
#pragma once

#include "LeCore/LeTexture/LeTexture.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeTextureDictionary : public LeBinaryChunk
{
public:
	LeTextureDictionary() = default;
	virtual ~LeTextureDictionary();

	LeTexture* FindTexture(uint64 nameHash);
	LeTexture* LoadTexture(const char* pTextureDir, const char* pTextureName);
	LeBool AddTexture(uint64 nameHash, LeTexture* pTexture);
	void PurgeAllTextures(void);

	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

private:
	std::unordered_map<uint64, LeTexture*> m_textures;

};