
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeMaterialVulkan/LeMaterialVulkan.h"

LeMaterialVk::LeMaterialVk()
{
	m_graphicsPipeline = NULL;
}

LeMaterialVk::~LeMaterialVk()
{
}


void LeMaterialVk::UpdateDescriptorSet(void)
{
	m_descriptorSet.UpdateSet(GetBaseTexture());
}