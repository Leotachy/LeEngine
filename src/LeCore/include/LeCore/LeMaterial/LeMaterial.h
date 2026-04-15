
#pragma once

#include "LeCore/LeRender/LeRenderResource.h"

#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{
	uint64 ShaderHash = 0;

	uint64 BaseTextureHash = 0;
};

#define MATERIAL_BASE_TEXTURE_DIRTY (1<<0)

#define LE_MATERIAL_MAX_NAME 128

class LeMaterial : public LeRenderResource
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial();
	static LeMaterial* Create(void);

	uint64 CalculateAttributeHash(void);

	void SetShader(LeShader* pShader) { m_shader = pShader; }
	LeShader* FindShader(void) const { return m_shader; }

	void SetBaseTexture(LeTexture* pTexture);
	LeTexture* GetBaseTexture(void) { return m_baseTexture; }


private:

	friend class LeMaterialDictionary;

	LeMaterialAttributes attribs;
	uint64 m_attribsHash;

	LeChar m_name[LE_MATERIAL_MAX_NAME];

	LeShader* m_shader;

	LeTexture* m_baseTexture;

};