

#include "LeCore/LeCore.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"

LeTexture::LeTexture()
{
	m_name[0] = '\0';
	m_pixels = NULL;
	m_width = m_height = 0;
}

LeTexture::~LeTexture()
{

}

LeTexture* LeTexture::Create(void)
{
	switch (LeGetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVulkanTexture();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeTexture::SetPixels(const LeRGBA* pPixels, int32 width, int32 height)
{
	LeFree(m_pixels);

	m_width = width;
	m_height = height;

	uint32 size = m_width * m_height * sizeof(LeRGBA);

	m_pixels = (LeRGBA*)LeMalloc(size);
	LeMemcpy(m_pixels, pPixels, size);
}