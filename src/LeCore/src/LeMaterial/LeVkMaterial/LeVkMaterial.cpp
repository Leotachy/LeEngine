
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeVkMaterial::LeVkMaterial()
{
	m_pGraphicsPipeline = NULL;
	m_hDescriptorSet = VK_NULL_HANDLE;

	m_bRefreshGraphicsPipeline = true;
}

LeVkMaterial::~LeVkMaterial()
{
}

void LeVkMaterial::OnDoubleSidedChanged(void)
{
	m_bRefreshGraphicsPipeline = true;
}