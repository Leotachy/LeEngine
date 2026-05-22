
#include "LeCore/lecore.h"

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"



void LeSerializableBinaryChunk::SerializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent)
{
	LE_DEBUG_ASSERT(pFileStream);


	ClearCache();

	m_parent = pParent;

	LeSerializableBinaryChunkType type = GetBinaryChunkType();
	pFileStream->Write(&type, sizeof(type));

	SerializeBinaryChunkImpl(pFileStream);

	m_parent = NULL;

	ClearCache();
}

void LeSerializableBinaryChunk::DeserializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent)
{
	LE_DEBUG_ASSERT(pFileStream);

	ClearCache();

	m_parent = pParent;

	LeSerializableBinaryChunkType type;
	pFileStream->Read(&type, sizeof(type));
	LE_ASSERT(GetBinaryChunkType() == type);

	DeserializeBinaryChunkImpl(pFileStream);

	m_parent = NULL;

	ClearCache();

}

LeSerializableBinaryChunk* LeSerializableBinaryChunk::FindParent(LeSerializableBinaryChunkType type)
{
	LeSerializableBinaryChunk* pScan = m_parent;
	while (pScan)
	{
		if (pScan->GetBinaryChunkType() == type)
			return pScan;

		pScan = pScan->m_parent;
	}

	return NULL;
}

LeSerializableBinaryChunk* LeSerializableBinaryChunk::FindParentCached(LeSerializableBinaryChunkType type)
{
	int32 i;
	for (i = 0; i < CACHE_SIZE; i++)
	{
		if (!m_cache[i])
			break;
	
		if (m_cache[i]->GetBinaryChunkType() == type)
			return m_cache[i];
	}

	LeSerializableBinaryChunk* pFound = FindParent(type);
	if (pFound == NULL)
		return NULL;

	if (m_last_written_cache_index >= CACHE_SIZE)
		m_last_written_cache_index = 0;

	m_cache[m_last_written_cache_index++] = pFound;

	return pFound;


}

void LeSerializableBinaryChunk::ClearCache(void)
{
	for (int32 i = 0; i < CACHE_SIZE; i++)
		m_cache[i] = NULL;

	m_last_written_cache_index = 0;
}