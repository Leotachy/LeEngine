
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

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

#define MATBLOCK_ALL_DIRTY_FLAGS (MATBLOCK_VERTS_DIRTY|MATBLOCK_FACES_DIRTY)

class LeMatBlock : public LeBinaryChunk
{
protected:
	LeMatBlock();
public:
	virtual ~LeMatBlock();
	static LeMatBlock* Create(void);

	uint64 GetMaterialHash(void);
	void SetMaterial(LeMaterial* m);
	LeMaterial* GetMaterial(void);

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

	std::vector<LeVertex> Vertices;
	std::vector<LeFace> Faces;

	uint32 flags;

protected:

	uint64 m_materialHash;
	LeMaterial* m_material;

private:

	friend class LeMesh;

};

class LeMesh : public LeBinaryChunk
{
protected:
	LeMesh();
public:
	virtual ~LeMesh();
	static LeMesh* Create(void);

	void AddMatblock(LeMatBlock* pMb);
	uint32 GetNumMatblocks(void);
	LeMatBlock* GetMatblock(uint32 i);
	void RemoveMatblock(uint32 i);
	void PurgeAllMatblocks(void);

	template<typename Func>
	void ForEachMatblock(Func&& func)
	{
		for (LeMatBlock* pMatblock : m_Matblocks)
		{
			func(pMatblock);
		}
	}

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

protected:


	std::vector<LeMatBlock*> m_Matblocks;

};