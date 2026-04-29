
#pragma once

#include "LeCore/LeTexture/LeTexture.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

#define TEXTURE_DICTIONARY_TEXTURES_DIRTY (1<<0)

class LeTextureDictionary : public LeBinaryChunk
{
public:
	LeTextureDictionary();
	virtual ~LeTextureDictionary();

	LeTexture* FindTexture(uint64 nameHash);
	LeTexture* LoadTexture(const char* pTextureDir, const char* pTextureName);
	LeBool AddTexture(uint64 nameHash, LeTexture* pTexture);
	void PurgeAllTextures(void);

	template<typename Func>
	void ForEachTexture(Func&& func);

	uint32 flags;

protected:
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

private:
	std::unordered_map<uint64, LeTexture*> m_textures;

};