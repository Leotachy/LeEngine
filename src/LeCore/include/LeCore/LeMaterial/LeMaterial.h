
#pragma once

#include "LeCore/LeRender/LeRenderResource.h"

#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{

};

class LeMaterial : public LeRenderResource
{
protected:
	LeMaterial();
public:
	virtual ~LeMaterial();
	static LeMaterial* Create(void);


	void SetShader(LeShader* pShader) { m_pShader = pShader; }
	LeShader* GetShader(void) const { return m_pShader; }

	void SetBaseTexture(LeTexture* pTexture) { m_pBaseTexture = pTexture; }
	LeTexture* GetBaseTexture(void) { return m_pBaseTexture; }

private:


	LeShader* m_pShader;

	LeTexture* m_pBaseTexture;

};