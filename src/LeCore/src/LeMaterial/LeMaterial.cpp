

#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeMaterial/LeVkMaterial/LeVkMaterial.h"

LeMaterial::LeMaterial()
{
	m_AttribsHash = 0;
	m_Name[0] = 0;

	m_Color = { 255, 255, 255, 255 };
	m_pShader = NULL;
	m_pBaseTexture = NULL;

	Flags = MATERIAL_ALL_DIRTY_FLAGS;
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
	if (Flags & MATERIAL_ATTRIB_HASH_DIRTY)
	{

		LeMaterialAttributes Attribs =
		{
			.Color = m_Color,
			.ShaderHash = m_pShader ? m_pShader->GetNameHash() : 0,
			.BaseTextureHash = m_pBaseTexture ? m_pBaseTexture->GetNameHash() : 0,
		};

		m_AttribsHash = LeHash::hash64_bin(&Attribs, sizeof(Attribs));
		Flags &= ~MATERIAL_ATTRIB_HASH_DIRTY;
	}
	
	return m_AttribsHash;
}

void LeMaterial::SetName(const LeChar* pName)
{
	LE_DEBUG_ASSERT(pName);

	strcpy(m_Name, pName);
}

void LeMaterial::SetColor(const LeRGBA color)
{
	m_Color = color;
	Flags |= MATERIAL_COLOR_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SetShader(LeShader* pShader)
{ 
	m_pShader = pShader;
	Flags |= MATERIAL_SHADER_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SetBaseTexture(LeTexture* pTexture)
{
	m_pBaseTexture = pTexture;
	Flags |= MATERIAL_BASE_TEXTURE_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	pFileStream->WriteString(m_Name);
	
	pFileStream->Write(&m_Color, sizeof(m_Color));
	uint64 ShaderHash = GetShader() ? GetShader()->GetNameHash() : 0;
	pFileStream->WriteNumber(&ShaderHash);
	uint64 BaseTextureHash = GetBaseTexture() ? GetBaseTexture()->GetNameHash() : 0;
	pFileStream->WriteNumber(&BaseTextureHash);
}

void LeMaterial::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	pFileStream->ReadString(m_Name, LE_MATERIAL_MAX_NAME);

	LeScene* pScene = static_cast<LeScene*>(FindParentCached(LeSerializableBinaryChunkType::Scene));
	LE_DEBUG_ASSERT(pScene);

	pFileStream->Read(&m_Color, sizeof(m_Color));
	uint64 ShaderHash;
	pFileStream->ReadNumber(&ShaderHash);

	// Specified shaders currently not implemented
	//if (ShaderHash)
	//	SetShader(LeRenderInterface::GetShaderDictionary()->FindShader(ShaderHash, LeShaderType::Fragment));


	uint64 BaseTextureHash;
	pFileStream->ReadNumber(&BaseTextureHash);
	if (BaseTextureHash)
		SetBaseTexture(pScene->GetTextureDictionary()->FindTexture(BaseTextureHash));
}