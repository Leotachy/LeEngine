
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeVkMaterial::LeVkMaterial()
{
	m_pGraphicsPipeline = NULL;
	m_hDescriptorSet = VK_NULL_HANDLE;
}

LeVkMaterial::~LeVkMaterial()
{
}