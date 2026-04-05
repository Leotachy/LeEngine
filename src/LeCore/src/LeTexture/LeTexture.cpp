
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeTexture/LeTexture.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"

LeTexture::LeTexture(const LeChar* pName)
{
	LE_DEBUG_ASSERT(pName);

	m_name = _strdup(pName);
	m_width = m_height = 0;
}

LeTexture::~LeTexture()
{
}

LeTexture* LeTexture::Create(const LeChar* pName)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkTexture(pName);
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}
