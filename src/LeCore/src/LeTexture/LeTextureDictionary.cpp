

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeTexture/LeTextureDictionary.h"

LeTextureDictionary::~LeTextureDictionary()
{
	PurgeAllTextures();
}

LeTexture* LeTextureDictionary::FindTexture(uint64 nameHash)
{
	LE_DEBUG_ASSERT(nameHash);

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

	LeTexture* pTexture = LeTexture::Create();
	pTexture->SetName(pTextureName);
	pTexture->SetPixels((LeRGBA*)pPixelData, w, h);
	pTexture->Compile();
	stbi_image_free(pPixelData);

	AddTexture(uNameHash, pTexture);

	return pTexture;
}

LeBool LeTextureDictionary::AddTexture(uint64 nameHash, LeTexture* pTexture)
{
	LE_DEBUG_ASSERT(nameHash);
	LE_DEBUG_ASSERT(pTexture);

	if (FindTexture(nameHash))
	{
		return false;
	}

	m_textures[nameHash] = pTexture;
	return true;
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

void LeTextureDictionary::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numTextures = m_textures.size();
	pFileStream->WriteNumber(&numTextures);

	for (auto pair : m_textures)
	{
		LeTexture* pTexture = pair.second;

		pFileStream->WriteString(pTexture->m_name);

		pFileStream->WriteNumber(&pTexture->m_width);
		pFileStream->WriteNumber(&pTexture->m_height);

		uint32 numPixels = pTexture->m_width * pTexture->m_height;

		pFileStream->Write(pTexture->m_pixels, numPixels);

		pFileStream->Write(&pTexture->attribs, sizeof(pTexture->attribs));
	}
}

void LeTextureDictionary::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numTextures;
	pFileStream->ReadNumber(&numTextures);

	for (uint32 i = 0; i < numTextures; i++)
	{
		LeTexture* pTexture = LeTexture::Create();

		pFileStream->ReadString(pTexture->m_name, LE_TEXTURE_MAX_NAME);

		pFileStream->ReadNumber(&pTexture->m_width);
		pFileStream->ReadNumber(&pTexture->m_height);

		uint32 numPixels = pTexture->m_width * pTexture->m_height;

		pTexture->m_pixels = new LeRGBA[numPixels];
		pFileStream->Read(pTexture->m_pixels, numPixels);

		pFileStream->Read(&pTexture->attribs, sizeof(pTexture->attribs));

		pTexture->Compile();
	}
}