
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

	uint32 GetNumVerts(void) { return (uint32)m_vertices.size(); }
	void SetNumVerts(uint32 n);
	LeVertex* GetVertexData(void) { return m_vertices.data(); }

	uint32 GetNumFaces(void) { return (uint32)m_faces.size(); }
	void SetNumFaces(uint32 n);
	LeFace* GetFaceData(void) { return m_faces.data(); }

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

	virtual void Compile(void) = 0;
	LeBool IsCompiled(void) { return m_isCompiled; }

	uint32 flags;

protected:

	LeMaterial* m_material;

	std::vector<LeVertex> m_vertices;
	std::vector<LeFace> m_faces;

	LeBool m_isCompiled;

private:
	uint64 m_materialHash; // during (de)serialization only!
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