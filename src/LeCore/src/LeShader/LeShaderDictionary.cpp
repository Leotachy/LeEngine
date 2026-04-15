
#include "LeCore/lecore.h"

#include "LeCore/LeShader/LeShaderDictionary.h"


LeShaderDictionary::LeShaderDictionary(void)
{

}


LeShaderDictionary::~LeShaderDictionary(void)
{
	PurgeAllShaders();
}

LeBool LeShaderDictionary::AddShader(LeShader* pShader)
{
	LE_DEBUG_ASSERT(pShader);

	if (LeShader* pShaderDupe = FindShader(pShader->GetNameHash()))
		return false;

	m_ShaderSet[pShader->GetNameHash()] = pShader;
	return true;
}

LeShader* LeShaderDictionary::FindShader(uint64 uNameHash)
{
	LE_DEBUG_ASSERT(uNameHash);

	auto found = m_ShaderSet.find(uNameHash);
	if (found != m_ShaderSet.end())
		return found->second;

	return NULL;
}

LeBool LeShaderDictionary::ShaderExists(uint64 uNameHash)
{
	LE_DEBUG_ASSERT(uNameHash);

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