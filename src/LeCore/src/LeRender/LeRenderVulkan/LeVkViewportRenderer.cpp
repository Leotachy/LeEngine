
#include "LeCore/lecore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeVkViewportRenderer.h"


void LeVkViewportRenderer::Init(uint32 width, uint32 height)
{
	m_width = width;
	m_height = height;

	for (uint32 i = 0; i < LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT; i++)
		m_viewportTextures[i] = NULL;

	m_viewportMaterial = new LeVkMaterial();
	m_viewportMaterial->attribs


	RebuildTextures();
}

void LeVkViewportRenderer::RebuildTextures(void)
{

}