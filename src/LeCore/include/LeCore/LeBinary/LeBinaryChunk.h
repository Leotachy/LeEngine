
#pragma once

#include "LeCore/LeStream/LeStreamFile.h"

enum LeBinaryChunkType
{
	BINARY_CHUNK_TYPE_OFFSET = 1245,
	BINARY_CHUNK_TYPE_SCENE,
};

enum LeBinaryChunkVersion
{
	BINARY_CHUNK_VER_OFFSET = 69,
	BINARY_CHUNK_VERSION_1,				// Initial version


	BINARY_CHUNK_CURRENT_VERSION = BINARY_CHUNK_VERSION_1
};

struct LeBinaryChunkHeader
{
	LeBinaryChunkType type;
	LeBinaryChunkVersion version;
};

class LeBinaryChunk
{
public:
	void SerializeBinaryChunk(LeStreamFile* pFileStream);
	void DeserializeBinaryChunk(LeStreamFile* pFileStream);
protected:

	LeBinaryChunk();
	virtual ~LeBinaryChunk() = default;

	virtual void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) = 0;
	virtual void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) = 0;

	LeBinaryChunkHeader m_header;
};