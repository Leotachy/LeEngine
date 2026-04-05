

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeTexture/LeTextureDictionary.h"

LeTextureDictionary& LeTextureDictionary::Singleton()
{
	static LeTextureDictionary TextureDictionary;
	return TextureDictionary;
}

LeTexture* LeTextureDictionary::FindTexture(uint64 nameHash)
{
	auto found = m_textures.find(nameHash);
	if (found != m_textures.end())
	{
		return found->second;
	}

	return NULL;
}

LeTexture* LeTextureDictionary::LoadTexture(const char* pTextureDir, const char* pTextureName)
{
	LE_DEBUG_ASSERT(pTextureDir);
	LE_DEBUG_ASSERT(pTextureName);

	uint64 uNameHash = lehash64(pTextureName);
	if (LeTexture* pFoundTexture = FindTexture(uNameHash))
		return pFoundTexture;

	LeChar FullTexturePath[2048];
	strcpy(FullTexturePath, pTextureDir);
	strcat(FullTexturePath, pTextureName);

	int32 w, h, ch;
	void* pPixelData = stbi_load(FullTexturePath, &w, &h, &ch, 4);

	LE_DEBUG_ASSERT(ch == 4);

	LeTexture* pTexture = LeTexture::Create(pTextureName);
	pTexture->SetPixels((LeRGBA*)pPixelData, w, h);
	stbi_image_free(pPixelData);

	AddTexture(uNameHash, pTexture);

	return pTexture;
}

void LeTextureDictionary::AddTexture(uint64 nameHash, LeTexture* pTexture)
{
	m_textures[nameHash] = pTexture;
}

void LeTextureDictionary::PurgeAllTextures(void)
{
	for (auto pair : m_textures)
	{
		LeTexture* pTexture = pair.second;
		delete pTexture;
	}

	m_textures.clear();
}

LeTextureDictionary::~LeTextureDictionary()
{
	PurgeAllTextures();
}