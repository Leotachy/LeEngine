
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


void LeWorld::SerializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::SerializeBinaryChunk(pFileStream);

	m_mesh->SerializeBinaryChunk(pFileStream);
}

void LeWorld::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{
	LeBinaryChunk::DeserializeBinaryChunk(pFileStream);

	m_mesh->DeserializeBinaryChunk(pFileStream);
}