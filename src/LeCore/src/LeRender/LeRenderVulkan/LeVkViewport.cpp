
#include "LeCore/lecore.h"

#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkViewport.h"
#include "LeCore/LeRender/LeRenderVulkan/LeVkUtils.h"


void LeVkViewport::Init(VkExtent2D extent)
{
	LE_DEBUG_ASSERT(extent.width > 0 && extent.height > 0);

	m_extent = extent;

	m_imagesValid = false;

	RebuildImages();

	LE_DEBUG_MESSAGE("LeVkViewport initialized");
}

void LeVkViewport::ReInit(VkExtent2D extent)
{
	LE_DEBUG_ASSERT(extent.width > 0 && extent.height > 0);

	m_extent = extent;

	RebuildImages();
}

void LeVkViewport::DeInit(void)
{
	if (m_imagesValid)
	{
		LeVkUtils::DestroyImage(&m_colorImage);
		LeVkUtils::DestroyImage(&m_depthImage);
	}

	m_imagesValid = false;

	LE_DEBUG_MESSAGE("LeVkViewport deinitialized");
}

void LeVkViewport::RebuildImages()
{
	
	if (m_imagesValid)
	{
		LeVkUtils::DestroyImage(&m_colorImage);
		LeVkUtils::DestroyImage(&m_depthImage);
	}

	VkExtent3D extent = VkExtent3D(m_extent.width, m_extent.height, 1);

	LeVkUtils::CreateImage(
		extent, LeVkRenderer::VIEWPORT_COLOR_IMAGE_FORMAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT		|
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_VIEW_TYPE_2D, 1, 1, &m_colorImage);

	LeVkUtils::CreateImage(
		extent, LeVkRenderer::VIEWPORT_DEPTH_IMAGE_FORMAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_VIEW_TYPE_2D, 1, 1, &m_depthImage);

	m_imagesValid = true;
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