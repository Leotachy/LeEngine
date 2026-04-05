
#pragma once

// Abstract class for texture dictionary

#include "LeCore/LeTexture/LeTexture.h"

class LeTextureDictionary
{
private:
	LeTextureDictionary();
	virtual ~LeTextureDictionary() = default;
public:
	LeTextureDictionary& Singleton();

	LeTexture* LoadTexture(const char* pTexturePath);

	void PurgeAllTextures(void);

};