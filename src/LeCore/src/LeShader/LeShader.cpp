
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeShader/LeVkShader/LeVkShader.h"



LeShader::LeShader(const char* pName, const char* pShaderBinaryPath, LeShaderType type)
{
	LE_ASSERT(pName);

	m_NameHash = LeHash::hash64(pName);
	m_ShaderType = type;
}

LeShader::~LeShader()
{
}

LeShader* LeShader::Create(const char* pName, const char* pShaderBinaryPath, LeShaderType type)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkShader(pName, pShaderBinaryPath, type);
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}


LeShaderType LeShader::GetShaderTypeByFileExtension(const LeChar* pExtension)
{
	LE_DEBUG_ASSERT(pExtension);

	if (strcmp(pExtension, ".vert") == 0)
		return LeShaderType::Vertex;

	if (strcmp(pExtension, ".frag") == 0)
		return LeShaderType::Fragment;

	return LeShaderType::NumShaderTypes;

}