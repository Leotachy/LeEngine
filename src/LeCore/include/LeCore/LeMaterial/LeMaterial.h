
#pragma once

#include "LeCore/LeBinary/LeBinaryChunk.h"
#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{
	uint64 ShaderHash = 0;

	uint64 BaseTextureHash = 0;
};

#define MATERIAL_ATTRIB_HASH_DIRTY (1<<0)
#define MATERIAL_BASE_TEXTURE_DIRTY (1<<1)
#define MATERIAL_SHADER_DIRTY (1<<2)

#define MATERIAL_ALL_DIRTY_FLAGS (MATERIAL_ATTRIB_HASH_DIRTY|MATERIAL_BASE_TEXTURE_DIRTY|MATERIAL_SHADER_DIRTY)

#define LE_MATERIAL_MAX_NAME 128

class LeMaterial : public LeBinaryChunk
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial() = default;
	static LeMaterial* Create(void);

	const LeMaterialAttributes* GetAttributes(void) { return &m_attribs; }
	uint64 GetAttributeHash(void);

	void SetName(const LeChar* pName);
	const LeChar* GetName(void) { return m_name; }

	void SetShader(LeShader* pShader);
	LeShader* GetShader(void) const { return m_shader; }

	void SetBaseTexture(LeTexture* pTexture);
	LeTexture* GetBaseTexture(void) { return m_baseTexture; }

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

	uint32 flags;

private:

	LeMaterialAttributes m_attribs;
	uint64 m_attribsHash;

	LeChar m_name[LE_MATERIAL_MAX_NAME];

	LeShader* m_shader;

	LeTexture* m_baseTexture;

};