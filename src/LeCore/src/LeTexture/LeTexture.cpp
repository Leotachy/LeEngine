
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeTextureVulkan/LeTextureVulkan.h"

LeTexture::LeTexture()
{
	m_name[0] = '\0';
	m_width = m_height = 0;
}

LeTexture::~LeTexture()
{
}

LeTexture* LeTexture::Create(void)
{
	switch (LeRenderInterface::LeGetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeTextureVk();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}
