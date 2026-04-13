
#pragma once

#include "LeCore/LeRender/LeRenderResource.h"

#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{
	uint64 ShaderHash;

	uint64 BaseTextureHash;
};

#define MATERIAL_BASE_TEXTURE_DIRTY (1<<0)

class LeMaterial : public LeRenderResource
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial();
	static LeMaterial* Create(void);

	uint64 CalculateAttributeHash(void);

	void SetShader(LeShader* pShader) { m_shader = pShader; }
	LeShader* GetShader(void) const { return m_shader; }

	void SetBaseTexture(LeTexture* pTexture);
	LeTexture* GetBaseTexture(void) { return m_baseTexture; }


private:

	LeMaterialAttributes attribs;
	uint64 m_attribsHash;

	LeShader* m_shader;

	LeTexture* m_baseTexture;

};