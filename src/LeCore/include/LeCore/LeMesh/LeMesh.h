
#pragma once

#include "LeCore/LeRender/LeRenderResource.h"

#include "LeCore/LeMaterial/LeMaterial.h"

struct LeVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
};

struct LeFace
{
	uint16 v1, v2, v3;
};

#define MATBLOCK_VERTS_DIRTY (1<<0) // The verts need to be reuploaded to the vertex buffer
#define MATBLOCK_FACES_DIRTY (1<<1) // The faces need to be reuploaded to the index buffer

class LeMatBlock : public LeRenderResource
{
protected:
	LeMatBlock();
public:
	virtual ~LeMatBlock();
	static LeMatBlock* Create(void);

	void SetMaterial(LeMaterial* m);
	LeMaterial* GetMaterial(void);


	std::vector<LeVertex> Vertices;
	std::vector<LeFace> Faces;

protected:

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
	void PurgeAllMatblocks(void);

private:

	std::vector<LeMatBlock*> m_Matblocks;

};