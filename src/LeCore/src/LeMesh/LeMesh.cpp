
#include "LeCore/LeCore.h"
#include "LeCore/LeRender/LeRender.h"
#include "LeCore/LeRender/LeRenderVulkan/LeRenderVulkan.h"
#include "LeCore/LeMesh/LeMesh.h"
#include "LeCore/LeMesh/LeVkMesh/LeVkMesh.h"

LeMatBlock::LeMatBlock()
{
	Flags = MATBLOCK_ALL_DIRTY_FLAGS;

	m_pMaterial = NULL;

	m_bIsCompiled = false;
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

void LeMatBlock::SetNumVerts(uint32 n)
{
	if (m_bIsCompiled)
		return;

	m_Vertices.resize(n);
}

void LeMatBlock::SetNumFaces(uint32 n)
{
	if (m_bIsCompiled)
		return;

	m_Faces.resize(n);
}

void LeMatBlock::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint64 materialHash = m_pMaterial ? m_pMaterial->GetAttributeHash() : 0;
	pFileStream->WriteNumber(&materialHash);

	uint32 numVerts = GetNumVerts();
	pFileStream->WriteNumber(&numVerts);
	pFileStream->Write(GetVertexData(), sizeof(LeVertex) * numVerts);

	uint32 numFaces = GetNumFaces();
	pFileStream->WriteNumber(&numFaces);
	pFileStream->Write(GetFaceData(), sizeof(LeFace) * numFaces);
}

void LeMatBlock::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{


	uint64 materialHash;
	pFileStream->ReadNumber(&materialHash);

	// We're gonna need the material dictionary for linking
	// Get it from the scene
	LeScene* pScene = static_cast<LeScene*>(FindParentCached(LeSerializableBinaryChunkType::Scene));
	LE_DEBUG_ASSERT(pScene);
	SetMaterial(pScene->GetMaterialDictionary()->FindMaterial(materialHash));

	uint32 numVerts;
	pFileStream->ReadNumber(&numVerts);
	SetNumVerts(numVerts);
	pFileStream->Read(GetVertexData(), sizeof(LeVertex) * numVerts);

	uint32 numFaces;
	pFileStream->ReadNumber(&numFaces);
	SetNumFaces(numVerts);
	pFileStream->Read(GetFaceData(), sizeof(LeFace) * numFaces);

	Compile();
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

void LeMesh::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMatblocks = (uint32)m_Matblocks.size();
	pFileStream->WriteNumber(&numMatblocks);

	for (LeMatBlock* pMatblock : m_Matblocks)
	{
		pMatblock->SerializeBinaryChunk(pFileStream, this);
	}
}

void LeMesh::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	uint32 numMatblocks;
	pFileStream->ReadNumber(&numMatblocks);

	for (uint32 i = 0; i < numMatblocks; i++)
	{
		LeMatBlock* pMatblock = LeMatBlock::Create();

		pMatblock->DeserializeBinaryChunk(pFileStream, this);

		AddMatblock(pMatblock);
	}
}