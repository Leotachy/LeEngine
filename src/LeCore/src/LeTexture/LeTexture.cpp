

#include "LeCore/LeCore.h"
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
	switch (LeGetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeTextureVk();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}
