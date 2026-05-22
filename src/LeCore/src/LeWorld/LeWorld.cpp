
#include "LeCore/lecore.h"

#include "LeCore/LeWorld/LeWorld.h"

LeWorld::LeWorld()
{
	m_mesh = LeMesh::Create();
}

LeWorld::~LeWorld(void)
{
	delete m_mesh;
	m_mesh = NULL;
}


void LeWorld::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_mesh->SerializeBinaryChunk(pFileStream, this);
}

void LeWorld::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_mesh->DeserializeBinaryChunk(pFileStream, this);
}