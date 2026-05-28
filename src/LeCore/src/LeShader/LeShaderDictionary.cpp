
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

LeShader* LeShaderDictionary::FindShader(uint64 uNameHash, LeShaderType type) const
{
	LE_DEBUG_ASSERT(uNameHash);

	auto found = m_Shaders[(uint32)type].find(uNameHash);
	if (found != m_Shaders[(uint32)type].end())
		return found->second;

	return NULL;
}

LeBool LeShaderDictionary::ShaderExists(uint64 uNameHash, LeShaderType type) const
{
	LE_DEBUG_ASSERT(uNameHash);

	return m_Shaders[(uint32)type].find(uNameHash) != m_Shaders[(uint32)type].end();
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
			LE_ASSERT(LeStore::DropExtension(FilenameNoExtension, pFileName, ".spv"));

			LeChar TypeExtension[512];
			if (LeStore::GetFileExtension(TypeExtension, FilenameNoExtension))
			{

				LeShaderType ShaderType = LeShader::GetShaderTypeByFileExtension(TypeExtension);

				if (ShaderType != LeShaderType::NumShaderTypes)
				{

					LeChar FilenameNoTypeExtension[512];
					LE_ASSERT(LeStore::DropExtension(FilenameNoTypeExtension, FilenameNoExtension, TypeExtension));

					LeShader* pShader = LeShader::Create(FilenameNoTypeExtension, FullFilepath, ShaderType);
					LE_DEBUG_ASSERT(pShader);

					if (AddShader(pShader))
						NumShadersLoaded++;
				}
				else
				{
					LE_WARN("Shader %s has an unrecognized type extension", pFileName);
				}
			}
			else
			{
				LE_WARN("Shader %s has no type extension", pFileName);
			}

			
		});

	LE_DEBUG_MESSAGE("Loaded %d shaders from directory: %s", NumShadersLoaded, pDirectoryPath);
}

void LeShaderDictionary::PurgeAllShaders(void)
{
	uint32 NumShaders = 0;
	for (auto& map : m_Shaders)
	{
		for (auto pair : map)
		{
			LeShader* pShader = pair.second;
			delete pShader;
		}

		NumShaders += (uint32)map.size();
		map.clear();
	}

	LE_DEBUG_MESSAGE("Purged %u shaders", NumShaders);
}

LeBool LeShaderDictionary::AddShader(LeShader* pShader)
{
	LE_DEBUG_ASSERT(pShader);

	if (LeShader* pShaderDupe = FindShader(pShader->GetNameHash(), pShader->GetType()))
	{
		return false;
	}

	uint32 setId = (uint32)pShader->GetType();

	m_Shaders[setId][pShader->GetNameHash()] = pShader;
	return true;
}