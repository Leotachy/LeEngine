
#include "LeCore/lecore.h"


#include "LeCore/LeRender/LeRenderVulkan/LeVkViewport.h"


void LeVkViewport::Init(VkExtent2D extent)
{
	LE_DEBUG_ASSERT(extent.width > 0 && extent.height > 0);

	m_extent = extent;

	m_colorTexture = NULL;
	m_depthTexture = NULL;

	RebuildTextures();
}

void LeVkViewport::ReInit(VkExtent2D extent)
{
	LE_DEBUG_ASSERT(extent.width > 0 && extent.height > 0);

	m_extent = extent;

	RebuildTextures();
}

void LeVkViewport::DeInit(void)
{
	delete m_colorTexture;
	m_colorTexture = NULL;

	delete m_depthTexture;
	m_depthTexture = NULL;
}

void LeVkViewport::RebuildTextures(void)
{
	delete m_colorTexture;
	m_colorTexture = new LeVkTexture(m_extent.width, m_extent.height);
	m_colorTexture->SetName("viewport_color");

	delete m_depthTexture;
	m_depthTexture = new LeVkTexture(m_extent.width, m_extent.height);
	m_depthTexture->SetName("viewport_depth");
}

//#ifdef SUPPORT_IMGUI
//if (bRenderToImGuiTexture)
//{
//	m_ViewportData.FrameData[i].ImGuiTexture = ImGui_ImplVulkan_AddTexture(m_ViewportData.Sampler,
//		m_ViewportData.FrameData[i].OutputImageView,
//		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//}
//else
//{
//	m_ViewportData.FrameData[i].ImGuiTexture = VK_NULL_HANDLE;
//}
//#endif