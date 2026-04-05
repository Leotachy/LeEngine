
#pragma once

// Abstract class for texture dictionary

#include "LeCore/LeTexture/LeTexture.h"

class LeTextureDictionary
{

public:
	static LeTextureDictionary& Singleton();

	LeTexture* FindTexture(uint64 nameHash);
	LeTexture* LoadTexture(const char* pTextureDir, const char* pTextureName);
	void AddTexture(uint64 nameHash, LeTexture* pTexture);
	void PurgeAllTextures(void);

protected:
	LeTextureDictionary() = default;
	virtual ~LeTextureDictionary();

	


	std::unordered_map<uint64, LeTexture*> m_textures;

};