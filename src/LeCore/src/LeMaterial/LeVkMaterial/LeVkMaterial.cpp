
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeVkMaterial::LeVkMaterial()
{
	m_graphicsPipeline = NULL;
	m_descriptorSet.Init();
}

LeVkMaterial::~LeVkMaterial()
{
}


void LeVkMaterial::UpdateDescriptorSet(void)
{
	m_descriptorSet.UpdateSet(GetBaseTexture());
}