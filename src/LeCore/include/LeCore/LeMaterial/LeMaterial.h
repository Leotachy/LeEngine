
#pragma once

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

#define LE_MATERIAL_MAX_NAME 128

class LeMaterial
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial();
	static LeMaterial* Create(void);

	uint64 GetAttributeHash(void);

	void SetShader(LeShader* pShader);
	LeShader* GetShader(void) const { return m_shader; }

	void SetBaseTexture(LeTexture* pTexture);
	LeTexture* GetBaseTexture(void) { return m_baseTexture; }

	uint32 flags;

private:

	friend class LeMaterialDictionary;

	LeMaterialAttributes m_attribs;
	uint64 m_attribsHash;

	LeChar m_name[LE_MATERIAL_MAX_NAME];

	LeShader* m_shader;

	LeTexture* m_baseTexture;

};