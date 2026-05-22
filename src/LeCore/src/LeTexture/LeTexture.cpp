
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

LeTexture::LeTexture()
{

	m_name[0] = 0;
	m_nameHash = 0;

	m_width = m_height = 0;
	m_pixels = NULL;

	flags = TEXTURE_ALL_DIRTY_FLAGS;
}

LeTexture::~LeTexture()
{
	LeFree(m_pixels);
	m_pixels = NULL;
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
	strncpy(m_name, pName, sizeof(m_name));
	m_nameHash = lehash64(m_name);
}


void LeTexture::AllocatePixels(uint32 width, uint32 height)
{
	if (m_pixels)
		return;

	m_width = width;
	m_height = height;

	int32 numPixels = m_width * m_height;
	m_pixels = new LeRGBA[numPixels];

	OnPixelsAllocated();
}

void LeTexture::SetPixels(const LeRGBA* pPixels)
{

	int32 numPixels = m_width * m_height;
	LeMemcpy(m_pixels, pPixels, numPixels * sizeof(LeRGBA));

	flags |= TEXTURE_PIXELS_DIRTY;
}

const LeRGBA* LeTexture::GetPixels(void) const
{
	return m_pixels;
}

void LeTexture::SetAttributes(LeTextureAttributes* pAttribs)
{
	LE_DEBUG_ASSERT(pAttribs);

	m_attribs = *pAttribs;

	flags |= TEXTURE_ATTRIBUTES_DIRTY;
}

void LeTexture::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{

	pFileStream->WriteString(m_name);

	pFileStream->WriteNumber(&m_width);
	pFileStream->WriteNumber(&m_height);
	uint32 numPixels = m_width * m_height;
	pFileStream->Write(m_pixels, numPixels * sizeof(LeRGBA));

	pFileStream->Write(&m_attribs, sizeof(m_attribs));
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
	pFileStream->Read(m_pixels, numPixels * sizeof(LeRGBA));
	
	pFileStream->Read(&m_attribs, sizeof(m_attribs));
}