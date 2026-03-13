
#pragma once

#include "LeCore/LeShader/LeShader.h"
#include "LeCore/LeTexture/LeTexture.h"

struct LeMaterialAttributes
{

};

#define MATERIAL_DIRTY (1<<1) // The material's attributes need to be reuploaded to the uniform buffer

class LeMaterial
{

public:

	LeMaterial();
	~LeMaterial();

	void AddFlags(uint32 f) { m_Flags |= f; }
	void SetFlags(uint32 f) { m_Flags = f; }
	uint32 GetFlags(void) const { return m_Flags; }

	void SetShader(LeShader* pShader) { m_pShader = pShader; }
	LeShader* GetShader(void) const { return m_pShader; }

private:

	uint32 m_Flags;

	LeShader* m_pShader;


};