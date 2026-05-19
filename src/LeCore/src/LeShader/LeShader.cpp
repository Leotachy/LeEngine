
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeShader/LeVkShader/LeVkShader.h"



LeShader::LeShader(const LeChar* name, const char* pShaderBinaryPath)
{
	LE_ASSERT(name);

	m_NameHash = lehash64(name);
}

LeShader::~LeShader()
{
}

LeShader* LeShader::Create(const char* pName, const char* pShaderBinaryPath)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::VULKAN:
		return new LeVkShader(pName, pShaderBinaryPath);
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}