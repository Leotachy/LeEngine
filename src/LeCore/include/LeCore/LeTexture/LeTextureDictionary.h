
#pragma once

#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

#define TEXTURE_DICTIONARY_TEXTURES_DIRTY (1<<0)

class LeTextureDictionary : public LeSerializableBinaryChunk
{
public:
	LeTextureDictionary();
	virtual ~LeTextureDictionary();

	LeTexture* FindTexture(uint64 nameHash);
	LeTexture* LoadTexture(const char* pTextureDir, const char* pTextureName);
	LeBool AddTexture(LeTexture* pTexture);
	void PurgeAllTextures(void);

	template<typename Func>
	void ForEachTexture(Func&& func)
	{
		for (auto pair : m_Textures)
		{
			LeTexture* pTexture = pair.second;
			func(pTexture);
		}
	}

	uint32 flags;

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::TextureDictionary; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;


private:
	std::unordered_map<uint64, LeTexture*> m_Textures;

};