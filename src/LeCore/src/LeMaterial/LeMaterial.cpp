

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeMaterial::LeMaterial()
{
	m_attribsHash = 0;
	m_name[0] = 0;

	m_shader = NULL;
	m_baseTexture = NULL;

	flags = MATERIAL_ALL_DIRTY_FLAGS;
}

LeMaterial* LeMaterial::Create(void)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkMaterial();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

uint64 LeMaterial::GetAttributeHash(void)
{
	if (flags & MATERIAL_ATTRIB_HASH_DIRTY)
	{

		LeMaterialAttributes Attribs =
		{
			.ShaderHash = m_shader ? m_shader->GetNameHash() : 0,
			.BaseTextureHash = m_baseTexture ? m_baseTexture->GetNameHash() : 0,
		};

		m_attribsHash = lehash64_bin(&Attribs, sizeof(Attribs));
		flags &= ~MATERIAL_ATTRIB_HASH_DIRTY;
	}
	
	return m_attribsHash;
}

void LeMaterial::SetName(const LeChar* pName)
{
	LE_DEBUG_ASSERT(pName);

	strcpy(m_name, pName);
}

void LeMaterial::SetShader(LeShader* pShader)
{ 
	m_shader = pShader;
	flags |= MATERIAL_SHADER_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SetBaseTexture(LeTexture* pTexture)
{
	m_baseTexture = pTexture;
	flags |= MATERIAL_BASE_TEXTURE_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	pFileStream->WriteString(m_name);
	
	uint64 ShaderHash = GetShader() ? GetShader()->GetNameHash() : 0;
	pFileStream->WriteNumber(&ShaderHash);
	uint64 BaseTextureHash = GetBaseTexture() ? GetBaseTexture()->GetNameHash() : 0;
	pFileStream->WriteNumber(&BaseTextureHash);
}

void LeMaterial::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	pFileStream->ReadString(m_name, LE_MATERIAL_MAX_NAME);

	LeScene* pScene = static_cast<LeScene*>(FindParentCached(LeSerializableBinaryChunkType::Scene));
	LE_DEBUG_ASSERT(pScene);

	uint64 ShaderHash;
	pFileStream->ReadNumber(&ShaderHash);
	if (ShaderHash)
		SetShader(LeRenderInterface::GetShaderDictionary()->FindShader(ShaderHash));

	uint64 BaseTextureHash;
	pFileStream->ReadNumber(&BaseTextureHash);
	if (BaseTextureHash)
		SetBaseTexture(pScene->GetTextureDictionary()->FindTexture(BaseTextureHash));
}