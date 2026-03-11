
#pragma once

#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeMaterial/LeMaterial.h"

struct LeVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
};

#define MATBLOCK_VERTS_DIRTY (1<<0) // The verts need to be reuploaded to the vertex buffer

class LeMatBlock
{
protected:
	LeMatBlock();
public:
	virtual ~LeMatBlock();
	static LeMatBlock* Create(void);

	void AddFlags(uint32 f) { m_Flags |= f; }
	void SetFlags(uint32 f) { m_Flags = f; }
	uint32 GetFlags(void) const { return m_Flags; }

	void SetMaterial(LeMaterial* m);
	const LeMaterial* GetMaterial(void);


	std::vector<LeVertex> Vertices;

protected:

	uint32 m_Flags;

	LeMaterial* m_pMaterial;

};

class LeMesh
{

public:

	LeMesh();
	~LeMesh();

	void AddMatblock(LeMatBlock* pMb);
	uint32 GetNumMatblocks(void);
	LeMatBlock* GetMatblock(uint32 i);
	void RemoveMatblock(uint32 i);
	void RemoveAllMatblocks(void);

private:

	std::vector<LeMatBlock*> m_Matblocks;

};