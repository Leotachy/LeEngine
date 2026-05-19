
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"

LeMatBlock::LeMatBlock()
{
	flags = MATBLOCK_ALL_DIRTY_FLAGS;

	m_materialHash = 0;
	m_material = NULL;
}

LeMatBlock::~LeMatBlock()
{

}

LeMatBlock* LeMatBlock::Create(void)
{

	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkMatBlock();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;

}


uint64 LeMatBlock::GetMaterialHash(void)
{
	return m_materialHash;
}

void LeMatBlock::SetMaterial(LeMaterial* m)
{
	m_material = m;
	m_materialHash = m ? m->GetAttributeHash() : 0;
}

LeMaterial* LeMatBlock::GetMaterial(void)
{
	return m_material;
}


void LeMatBlock::SerializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	pFileStream->WriteNumber(&m_materialHash);

	uint32 numVerts = (uint32)Vertices.size();
	pFileStream->WriteNumber(&numVerts);

	for (uint32 i = 0; i < numVerts; i++)
	{
		LeVertex* pVertex = &Vertices[i];
		pFileStream->Write(pVertex, sizeof(LeVertex));
	}

	uint32 numFaces = (uint32)Faces.size();
	pFileStream->WriteNumber(&numFaces);

	for (uint32 i = 0; i < numFaces; i++)
	{
		LeFace* pFace = &Faces[i];
		pFileStream->Write(pFace, sizeof(LeFace));
	}
}

void LeMatBlock::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::DeserializeBinaryChunk(pFileStream);

	pFileStream->ReadNumber(&m_materialHash);

	uint32 numVerts;
	pFileStream->ReadNumber(&numVerts);
	Vertices.resize(numVerts);

	for (uint32 i = 0; i < numVerts; i++)
	{
		pFileStream->Read(&Vertices[i], sizeof(LeVertex));
	}

	uint32 numFaces;
	pFileStream->ReadNumber(&numFaces);
	Faces.resize(numFaces);

	for (uint32 i = 0; i < numFaces; i++)
	{
		pFileStream->Read(&Faces[i], sizeof(LeFace));
	}
}



LeMesh::LeMesh()
{

}

LeMesh::~LeMesh()
{
	PurgeAllMatblocks();
}

LeMesh* LeMesh::Create(void)
{
	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::Vulkan:
		return new LeVkMesh();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;
}

void LeMesh::AddMatblock(LeMatBlock* pMb)
{
	m_Matblocks.push_back(pMb);
}

uint32 LeMesh::GetNumMatblocks(void)
{ 
	return uint32(m_Matblocks.size()); 

}

LeMatBlock* LeMesh::GetMatblock(uint32 i)
{
	LE_DEBUG_ASSERT(i < m_Matblocks.size());
	return m_Matblocks[i];
}

void LeMesh::RemoveMatblock(uint32 i)
{
	LE_DEBUG_ASSERT(i < m_Matblocks.size());
	m_Matblocks.erase(m_Matblocks.begin() + i);
}

void LeMesh::PurgeAllMatblocks(void)
{
	for (LeMatBlock* pMb : m_Matblocks)
		delete pMb;

	m_Matblocks.clear();
}

void LeMesh::SerializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	uint32 numMatblocks = (uint32)m_Matblocks.size();
	pFileStream->WriteNumber(&numMatblocks);

	for (LeMatBlock* pMatblock : m_Matblocks)
	{
		pMatblock->SerializeBinaryChunk(pFileStream);
	}
}

void LeMesh::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{

	LeBinaryChunk::DeserializeBinaryChunk(pFileStream);

	uint32 numMatblocks;
	pFileStream->ReadNumber(&numMatblocks);

	for (uint32 i = 0; i < numMatblocks; i++)
	{
		LeMatBlock* pMatblock = LeMatBlock::Create();

		pMatblock->DeserializeBinaryChunk(pFileStream);

		AddMatblock(pMatblock);
	}
}