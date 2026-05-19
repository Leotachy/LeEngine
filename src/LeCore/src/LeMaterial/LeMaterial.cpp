

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
	case LeRenderApi::VULKAN:
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
		m_attribsHash = lehash64_bin(&m_attribs, sizeof(m_attribs));
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
	m_attribs.ShaderHash = pShader ? pShader->GetNameHash() : 0;
	flags |= MATERIAL_SHADER_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SetBaseTexture(LeTexture* pTexture)
{
	m_baseTexture = pTexture;
	m_attribs.BaseTextureHash = pTexture ? pTexture->GetNameHash() : 0;
	flags |= MATERIAL_BASE_TEXTURE_DIRTY | MATERIAL_ATTRIB_HASH_DIRTY;
}

void LeMaterial::SerializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	pFileStream->WriteString(m_name);
	pFileStream->Write(&m_attribs, sizeof(m_attribs));
}

void LeMaterial::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	pFileStream->ReadString(m_name, LE_MATERIAL_MAX_NAME);
	pFileStream->Read(&m_attribs, sizeof(m_attribs));
}