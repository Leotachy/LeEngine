
#pragma once

#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

class LeVkViewport
{
public:
	void Init(VkExtent2D extent);
	void ReInit(VkExtent2D extent);
	void DeInit(void);

	VkExtent2D GetExtent(void) const { return m_extent; }

	LeVkTexture* GetTexture(void) { return m_colorTexture; }
	LeVkTexture* GetDepthTexture(void) { return m_depthTexture; }

private:

	void RebuildTextures(void);

	VkExtent2D m_extent;

	LeVkTexture* m_colorTexture;
	LeVkTexture* m_depthTexture;
};