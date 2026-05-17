
#pragma once

#include "LeCore/LeRender/LeRenderVulkan/LeVkPhysicalDevice.h"
#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

class LeVkViewport
{
public:
	void Init(VkExtent2D extent);
	void ReInit(VkExtent2D extent);
	void DeInit(void);

	VkExtent2D GetExtent(void) const { return m_extent; }

	VkImage GetColorImage(void) { return m_colorImage.image; }
	VkImageView GetColorImageView(void) { return m_colorView; }

	VkImage GetDepthImage(void) { return m_depthImage.image; }
	VkImageView GetDepthImageView(void) { return m_depthView; }

private:

	void RebuildImages();


	VkExtent2D m_extent;

	LeBool m_imagesValid;

	LeVkMemoryImage m_colorImage;
	VkImageView m_colorView;

	LeVkMemoryImage m_depthImage;
	VkImageView m_depthView;
};