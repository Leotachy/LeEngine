
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"

LeMatBlock::LeMatBlock()
{
	flags = 0;

	m_material = NULL;
}

LeMatBlock::~LeMatBlock()
{

}

LeMatBlock* LeMatBlock::Create(void)
{

	switch (LeRenderInterface::GetRenderApi())
	{
	case LeRenderApi::LE_RENDER_API_VULKAN:
		return new LeVkMatBlock();
		break;
	}

	LE_DEBUG_ASSERT(0);
	return NULL;

}



void LeMatBlock::SetMaterial(LeMaterial* m)
{
	m_material = m;
}

LeMaterial* LeMatBlock::GetMaterial(void)
{
	return m_material;
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
	case LeRenderApi::LE_RENDER_API_VULKAN:
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

void LeMesh::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMatblocks = (uint32)m_Matblocks.size();
	pFileStream->WriteNumber(&numMatblocks);

	for (LeMatBlock* pMatblock : m_Matblocks)
	{
		pFileStream->WriteNumber(&pMatblock->m_materialHash);

		uint32 numVerts = (uint32)pMatblock->Vertices.size();
		pFileStream->WriteNumber(&numVerts);

		for (uint32 i = 0; i < numVerts; i++)
		{
			LeVertex* pVertex = &pMatblock->Vertices[i];
			pFileStream->Write(pVertex, sizeof(LeVertex));
		}

		uint32 numFaces = (uint32)pMatblock->Faces.size();
		pFileStream->WriteNumber(&numFaces);

		for (uint32 i = 0; i < numFaces; i++)
		{
			LeFace* pFace = &pMatblock->Faces[i];
			pFileStream->Write(pFace, sizeof(LeFace));
		}
	}
}

void LeMesh::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMatblocks;
	pFileStream->ReadNumber(&numMatblocks);

	for (uint32 i = 0; i < numMatblocks; i++)
	{
		LeMatBlock* pMatblock = LeMatBlock::Create();

		pFileStream->ReadNumber(&pMatblock->m_materialHash);

		uint32 numVerts;
		pFileStream->ReadNumber(&numVerts);
		pMatblock->Vertices.resize(numVerts);

		for (uint32 i = 0; i < numVerts; i++)
		{
			pFileStream->Read(&pMatblock->Vertices[i], sizeof(LeVertex));
		}

		uint32 numFaces;
		pFileStream->ReadNumber(&numFaces);
		pMatblock->Faces.resize(numFaces);

		for (uint32 i = 0; i < numFaces; i++)
		{
			pFileStream->Read(&pMatblock->Faces[i], sizeof(LeFace));
		}
	}
}