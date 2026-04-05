

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeMaterial::LeMaterial()
{
	m_shader = NULL;
	m_baseTexture = NULL;
}

LeMaterial::~LeMaterial()
{

}

LeMaterial* LeMaterial::Create(void)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkMaterial();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeMaterial::SetBaseTexture(LeTexture* pTexture)
{
	m_baseTexture = pTexture;
	AddFlags(MATERIAL_BASE_TEXTURE_DIRTY);
}