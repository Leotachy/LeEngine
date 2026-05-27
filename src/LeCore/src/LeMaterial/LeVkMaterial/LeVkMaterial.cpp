
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeVkMaterial::LeVkMaterial()
{
	m_pGraphicsPipeline = NULL;
	m_DescriptorSet.Init();
}

LeVkMaterial::~LeVkMaterial()
{
}


void LeVkMaterial::UpdateDescriptorSet(void)
{
	m_DescriptorSet.UpdateSet(GetBaseTexture());
}