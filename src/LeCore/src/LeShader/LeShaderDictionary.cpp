
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

LeShader* LeShaderDictionary::FindShader(uint64 uNameHash) const
{
	LE_DEBUG_ASSERT(uNameHash);

	auto found = m_Shaders.find(uNameHash);
	if (found != m_Shaders.end())
		return found->second;

	return NULL;
}

LeBool LeShaderDictionary::ShaderExists(uint64 uNameHash) const
{
	LE_DEBUG_ASSERT(uNameHash);

	return m_Shaders.find(uNameHash) != m_Shaders.end();
}

void LeShaderDictionary::LoadAllShaders(const LeChar* pDirectoryPath)
{
	LE_DEBUG_ASSERT(pDirectoryPath);

	int32 NumShadersLoaded = 0;

	LeChar buf[1024];
	strcpy(buf, pDirectoryPath);
	strcat(buf, "*.spv");

	LeStore::ForEachFileInDirectory(buf, [this, pDirectoryPath, &NumShadersLoaded](const LeChar* pFileName)
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

	uint32 NumShaders = (uint32)m_Shaders.size();

	for (auto pair : m_Shaders)
	{
		LeShader* pShader = pair.second;
		delete pShader;
	}

	m_Shaders.clear();

	LE_DEBUG_MESSAGE("Purged %u shaders", NumShaders);
}

LeBool LeShaderDictionary::AddShader(LeShader* pShader)
{
	LE_DEBUG_ASSERT(pShader);

	if (LeShader* pShaderDupe = FindShader(pShader->GetNameHash()))
		delete pShaderDupe;

	m_Shaders[pShader->GetNameHash()] = pShader;
	return true;
}