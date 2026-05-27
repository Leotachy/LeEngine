

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeTexture/LeTextureDictionary.h"

LeTextureDictionary::LeTextureDictionary()
{
	flags = TEXTURE_DICTIONARY_TEXTURES_DIRTY;
}

LeTextureDictionary::~LeTextureDictionary()
{
	PurgeAllTextures();
}

LeTexture* LeTextureDictionary::FindTexture(uint64 nameHash)
{
	LE_DEBUG_ASSERT(nameHash);

	auto found = m_Textures.find(nameHash);
	if (found != m_Textures.end())
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

	LeTexture* pTexture = LeTexture::Create();
	pTexture->SetName(pTextureName);
	pTexture->AllocatePixels(w, h);
	pTexture->SetPixels((LeRGBA*)pPixelData);
	stbi_image_free(pPixelData);

	AddTexture(pTexture);

	return pTexture;
}

LeBool LeTextureDictionary::AddTexture(LeTexture* pTexture)
{
	LE_DEBUG_ASSERT(pTexture);

	if (FindTexture(pTexture->GetNameHash()))
	{
		return false;
	}

	m_Textures[pTexture->GetNameHash()] = pTexture;
	return true;
}

void LeTextureDictionary::PurgeAllTextures(void)
{
	for (auto pair : m_Textures)
	{
		LeTexture* pTexture = pair.second;
		delete pTexture;
	}

	m_Textures.clear();
}

void LeTextureDictionary::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numTextures = (uint32)m_Textures.size();
	pFileStream->WriteNumber(&numTextures);

	for (auto pair : m_Textures)
	{
		LeTexture* pTexture = pair.second;
		pTexture->SerializeBinaryChunk(pFileStream, this);
	}
}

void LeTextureDictionary::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numTextures;
	pFileStream->ReadNumber(&numTextures);

	for (uint32 i = 0; i < numTextures; i++)
	{
		LeTexture* pTexture = LeTexture::Create();
		pTexture->DeserializeBinaryChunk(pFileStream, this);

		if (!AddTexture(pTexture))
			delete pTexture;
	}
}