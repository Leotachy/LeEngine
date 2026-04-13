
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

LeTexture::LeTexture()
{
	m_name[0] = 0;

	m_width = m_height = 0;
}

LeTexture::~LeTexture()
{
}

LeTexture* LeTexture::Create()
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkTexture();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeTexture::SetName(const LeChar* pName)
{
	strncpy(m_name, pName, sizeof(m_name));
}


void LeTexture::SetPixels(const LeRGBA* pPixels, uint32 width, uint32 height)
{

	ClearPixels();

	int32 numPixels = width * height;
	LE_DEBUG_ASSERT(numPixels);

	m_pixels = new LeRGBA[numPixels];
	LeMemcpy(m_pixels, pPixels, numPixels * sizeof(LeRGBA));

}

void LeTexture::ClearPixels(void)
{
	LeFree(m_pixels);
	m_pixels = NULL;
	m_width = m_height = 0;
}

LeBool LeTexture::ArePixelsSet(void) const
{
	return (m_width * m_height) > 0;
}

const LeRGBA* LeTexture::GetPixels(void)
{
	return m_pixels;
}