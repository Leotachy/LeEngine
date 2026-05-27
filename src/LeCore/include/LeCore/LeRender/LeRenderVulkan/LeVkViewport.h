
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

	VkExtent2D GetExtent(void) const { return m_Extent; }

	VkImage GetColorImage(void) { return m_ColorImage.image; }
	VkImageView GetColorImageView(void) { return m_ColorImage.imageView; }

	VkImage GetDepthImage(void) { return m_DepthImage.image; }
	VkImageView GetDepthImageView(void) { return m_DepthImage.imageView; }

private:

	void RebuildImages();


	VkExtent2D m_Extent;

	LeBool m_bImagesValid;

	LeVkAllocatedImage m_ColorImage;
	LeVkAllocatedImage m_DepthImage;
};