
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

LeTexture::LeTexture(uint32 width, uint32 height)
{

	LE_DEBUG_ASSERT(width > 0 && height > 0);

	m_name[0] = 0;

	m_width = width;
	m_height = height;

	int32 numPixels = m_width * m_height;
	m_pixels = new LeRGBA[numPixels];

	flags = 0;
}

LeTexture::~LeTexture()
{
	LeFree(m_pixels);
	m_pixels = NULL;
}

LeTexture* LeTexture::Create(uint32 width, uint32 height)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkTexture(width, height);
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


void LeTexture::SetPixels(const LeRGBA* pPixels)
{
	int32 numPixels = m_width * m_height;
	LeMemcpy(m_pixels, pPixels, numPixels * sizeof(LeRGBA));
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