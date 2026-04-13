
#include "LeCore/lecore.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"


LeBinaryChunk::LeBinaryChunk()
{
	m_header.type = LeBinaryChunkType::BINARY_CHUNK_TYPE_OFFSET;
	m_header.version = LeBinaryChunkVersion::BINARY_CHUNK_CURRENT_VERSION;
}

void LeBinaryChunk::SerializeBinaryChunk(LeStreamFile* pFileStream)
{
	LE_DEBUG_ASSERT(pFileStream);

	pFileStream->WriteNumber((uint32*)(&m_header.type));
	pFileStream->WriteNumber((uint32*)(&m_header.version));

	SerializeBinaryChunkImpl(pFileStream);
}

void LeBinaryChunk::DeserializeBinaryChunk(LeStreamFile* pFileStream)
{
	LE_DEBUG_ASSERT(pFileStream);

	pFileStream->ReadNumber((uint32*)(&m_header.type));
	pFileStream->ReadNumber((uint32*)(&m_header.version));

	DeserializeBinaryChunkImpl(pFileStream);

	m_header.version = BINARY_CHUNK_CURRENT_VERSION;
}