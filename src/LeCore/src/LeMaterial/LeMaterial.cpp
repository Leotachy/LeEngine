

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeMaterial/LeMaterialVulkan/LeMaterialVulkan.h"

LeMaterial::LeMaterial()
{
	m_pShader = NULL;
	m_pBaseTexture = NULL;
}

LeMaterial::~LeMaterial()
{

}

LeMaterial* LeMaterial::Create(void)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeMaterialVk();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}