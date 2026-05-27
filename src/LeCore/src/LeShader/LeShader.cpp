
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeShader/LeVkShader/LeVkShader.h"



LeShader::LeShader(const char* pName, const char* pShaderBinaryPath)
{
	LE_ASSERT(pName);

	m_NameHash = lehash64(pName);
}

LeShader::~LeShader()
{
}

LeShader* LeShader::Create(const char* pName, const char* pShaderBinaryPath)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkShader(pName, pShaderBinaryPath);
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}