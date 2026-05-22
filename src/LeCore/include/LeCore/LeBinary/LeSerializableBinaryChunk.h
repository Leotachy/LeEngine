
#pragma once

#include "LeCore/LeStream/LeStreamFile.h"

enum class LeSerializableBinaryChunkType
{
	None = 0x6000,
	Scene,
	TextureDictionary,
	Texture,
	MaterialDictionary,
	Material,
	World,
	Mesh,
	MatBlock

};

class LeSerializableBinaryChunk
{
public:

	virtual void SerializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent);
	virtual void DeserializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent);

protected:

	virtual LeSerializableBinaryChunkType GetBinaryChunkType(void) = 0;
	LeSerializableBinaryChunk* FindParent(LeSerializableBinaryChunkType type);
	LeSerializableBinaryChunk* FindParentCached(LeSerializableBinaryChunkType type);

private:

	virtual void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) = 0;
	virtual void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) = 0;

	void ClearCache(void);

	LeSerializableBinaryChunk* m_parent;

	static const int32 CACHE_SIZE = 3;
	LeSerializableBinaryChunk* m_cache[CACHE_SIZE];
	int32 m_last_written_cache_index;
};