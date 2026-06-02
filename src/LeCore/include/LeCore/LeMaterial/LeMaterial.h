
#pragma once

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"
#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{
	LeRGBA Color = {255, 255, 255, 255};
	uint64 ShaderHash = 0;
	uint64 BaseTextureHash = 0;
};

#define MATERIAL_ATTRIB_HASH_DIRTY	(1<<0)
#define MATERIAL_BASE_TEXTURE_DIRTY (1<<1)
#define MATERIAL_COLOR_DIRTY		(1<<2)
#define MATERIAL_SHADER_DIRTY		(1<<3)

#define MATERIAL_ALL_DIRTY_FLAGS (MATERIAL_ATTRIB_HASH_DIRTY|MATERIAL_BASE_TEXTURE_DIRTY|MATERIAL_COLOR_DIRTY|MATERIAL_SHADER_DIRTY)

#define LE_MATERIAL_MAX_NAME 128

class LeMaterial : public LeSerializableBinaryChunk
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial() = default;
	static LeMaterial* Create(void);

	uint64 GetAttributeHash(void);

	void SetName(const LeChar* pName);
	const LeChar* GetName(void) { return m_Name; }

	void SetColor(const LeRGBA color);
	LeRGBA GetColor(void) { return m_Color; }

	void SetShader(LeShader* pShader);
	LeShader* GetShader(void) { return m_pShader; }

	void SetBaseTexture(LeTexture* pTexture);
	LeTexture* GetBaseTexture(void) { return m_pBaseTexture; }

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::Material; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

	uint32 Flags;

private:

	uint64 m_AttribsHash;

	LeChar m_Name[LE_MATERIAL_MAX_NAME];

	LeRGBA m_Color;
	LeShader* m_pShader;
	LeTexture* m_pBaseTexture;

};