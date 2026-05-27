
#pragma once

#include "LeCore/LeMaterial/LeMaterial.h"
#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

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

class LeMatBlock : public LeSerializableBinaryChunk
{
protected:
	LeMatBlock();
public:
	virtual ~LeMatBlock();
	static LeMatBlock* Create(void);

	void SetMaterial(LeMaterial* m) { m_pMaterial = m; }
	LeMaterial* GetMaterial(void) { return m_pMaterial; }

	uint32 GetNumVerts(void) { return (uint32)m_Vertices.size(); }
	void SetNumVerts(uint32 n);
	LeVertex* GetVertexData(void) { return m_Vertices.data(); }

	uint32 GetNumFaces(void) { return (uint32)m_Faces.size(); }
	void SetNumFaces(uint32 n);
	LeFace* GetFaceData(void) { return m_Faces.data(); }

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::MatBlock; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

	virtual void Compile(void) = 0;
	LeBool IsCompiled(void) { return m_bIsCompiled; }

	uint32 Flags;

protected:

	LeMaterial* m_pMaterial;

	std::vector<LeVertex> m_Vertices;
	std::vector<LeFace> m_Faces;

	LeBool m_bIsCompiled;

private:

};

class LeMesh : public LeSerializableBinaryChunk
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

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::Mesh; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

protected:


	std::vector<LeMatBlock*> m_Matblocks;

};