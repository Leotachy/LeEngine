
#include "LeCore/lecore.h"

#include "LeCore/LeWorld/LeWorld.h"

LeWorld::LeWorld()
{
	m_pMesh = LeMesh::Create();
}

LeWorld::~LeWorld(void)
{
	delete m_pMesh;
	m_pMesh = NULL;
}


void LeWorld::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_pMesh->SerializeBinaryChunk(pFileStream, this);
}

void LeWorld::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_pMesh->DeserializeBinaryChunk(pFileStream, this);
}