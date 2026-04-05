
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
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkShader(pName, pShaderBinaryPath);
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeShaderDictionary::AddShader(LeShader* pShader)
{
	if (LeShader* pShaderDupe = GetShader(pShader->GetNameHash()))
	{
		delete pShaderDupe;
		m_ShaderSet[pShader->GetNameHash()] = pShader;
	}
}

LeShader* LeShaderDictionary::GetShader(uint64 uNameHash)
{
	auto found = m_ShaderSet.find(uNameHash);
	if (found != m_ShaderSet.end())
	{
		return found->second;
	}

	return NULL;
}

LeBool LeShaderDictionary::ShaderExists(uint64 uNameHash)
{
	return m_ShaderSet.find(uNameHash) != m_ShaderSet.end();
}

void LeShaderDictionary::PurgeAllShaders(void)
{
	for (auto pair : m_ShaderSet)
	{
		LeShader* pShader = pair.second;
		delete pShader;
	}

	m_ShaderSet.clear();
}


static LeShaderDictionary s_ShaderDictionary;

LeShaderDictionary* LeGetCurrentShaderDictionary(void)
{
	return &s_ShaderDictionary;
}