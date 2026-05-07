
#pragma once

#include "LeCore/LeTexture/LeVkTexture/LeVkTexture.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

class LeVkViewportRenderer
{
public:
	void Init(uint32 width, uint32 height);
	void ReInit(uint32 width, uint32 height);
	void DeInit(void);
private:

	void RebuildTextures(void);

	uint32 m_width, m_height;


	LeVkTexture* m_viewportTextures[LECORE_VULKAN_MAX_FRAMES_IN_FLIGHT];
	LeVkMaterial* m_viewportMaterial;
};