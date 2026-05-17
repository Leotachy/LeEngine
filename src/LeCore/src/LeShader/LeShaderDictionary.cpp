
#include "LeCore/lecore.h"

#include "LeCore/LeShader/LeShaderDictionary.h"
#include "LeCore/LeStore/LeStore.h"

LeShaderDictionary::LeShaderDictionary(void)
{

}

LeShaderDictionary::~LeShaderDictionary(void)
{
	PurgeAllShaders();
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

void LeShaderDictionary::LoadAllShaders(const LeChar* pDirectoryPath)
{
	LE_DEBUG_ASSERT(pDirectoryPath);

	int32 NumShadersLoaded = 0;

	LeStore::ForEachFileInDirectoryWithExtension(pDirectoryPath, ".spv", [this, pDirectoryPath, &NumShadersLoaded](const LeChar* pFileName)
		{

			LeChar FullFilepath[2048];
			strcpy(FullFilepath, pDirectoryPath);
			strcat(FullFilepath, pFileName);

			LeChar FilenameNoExtension[512];
			LeStore::DropExtension(FilenameNoExtension, pFileName, ".spv");

			LeShader* pShader = LeShader::Create(FilenameNoExtension, FullFilepath);
			LE_DEBUG_ASSERT(pShader);

			AddShader(pShader);

			NumShadersLoaded++;
		});

	LE_DEBUG_MESSAGE("Loaded %d shaders from directory: %s", NumShadersLoaded, pDirectoryPath);
}

void LeShaderDictionary::PurgeAllShaders(void)
{

	uint32 NumShaders = (uint32)m_ShaderSet.size();

	for (auto pair : m_ShaderSet)
	{
		LeShader* pShader = pair.second;
		delete pShader;
	}

	m_ShaderSet.clear();

	LE_DEBUG_MESSAGE("Purged %u shaders", NumShaders);
}

LeBool LeShaderDictionary::AddShader(LeShader* pShader)
{
	LE_DEBUG_ASSERT(pShader);

	if (LeShader* pShaderDupe = FindShader(pShader->GetNameHash()))
		delete pShaderDupe;

	m_ShaderSet[pShader->GetNameHash()] = pShader;
	return true;
}