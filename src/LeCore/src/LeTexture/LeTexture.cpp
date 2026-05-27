
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

LeTexture::LeTexture()
{

	m_Name[0] = 0;
	m_NameHash = 0;

	m_Width = m_Height = 0;
	m_pPixels = NULL;

	Flags = TEXTURE_ALL_DIRTY_FLAGS;
}

LeTexture::~LeTexture()
{
	LeFree(m_pPixels);
	m_pPixels = NULL;
}

LeTexture* LeTexture::Create()
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkTexture();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeTexture::SetName(const LeChar* pName)
{
	strncpy(m_Name, pName, sizeof(m_Name));
	m_NameHash = lehash64(m_Name);
}


void LeTexture::AllocatePixels(uint32 width, uint32 height)
{
	if (m_pPixels)
		return;

	m_Width = width;
	m_Height = height;

	int32 numPixels = m_Width * m_Height;
	m_pPixels = new LeRGBA[numPixels];

	OnPixelsAllocated();
}

void LeTexture::SetPixels(const LeRGBA* pPixels)
{

	int32 numPixels = m_Width * m_Height;
	LeMemcpy(m_pPixels, pPixels, numPixels * sizeof(LeRGBA));

	Flags |= TEXTURE_PIXELS_DIRTY;
}

const LeRGBA* LeTexture::GetPixels(void) const
{
	return m_pPixels;
}

void LeTexture::SetAttributes(LeTextureAttributes* pAttribs)
{
	LE_DEBUG_ASSERT(pAttribs);

	m_Attribs = *pAttribs;

	Flags |= TEXTURE_ATTRIBUTES_DIRTY;
}

void LeTexture::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{

	pFileStream->WriteString(m_Name);

	pFileStream->WriteNumber(&m_Width);
	pFileStream->WriteNumber(&m_Height);
	uint32 numPixels = m_Width * m_Height;
	pFileStream->Write(m_pPixels, numPixels * sizeof(LeRGBA));

	pFileStream->Write(&m_Attribs, sizeof(m_Attribs));
}

void LeTexture::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{

	LeChar NameBuffer[LE_TEXTURE_MAX_NAME];
	pFileStream->ReadString(NameBuffer, LE_TEXTURE_MAX_NAME);
	SetName(NameBuffer);

	uint32 width, height;
	pFileStream->ReadNumber(&width);
	pFileStream->ReadNumber(&height);
	AllocatePixels(width, height);

	uint32 numPixels = width * height;
	pFileStream->Read(m_pPixels, numPixels * sizeof(LeRGBA));
	
	pFileStream->Read(&m_Attribs, sizeof(m_Attribs));
}