
#include "LeCore/lecore.h"

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"



void LeSerializableBinaryChunk::SerializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent)
{
	LE_DEBUG_ASSERT(pFileStream);


	ClearCache();

	m_pParent = pParent;

	LeSerializableBinaryChunkType type = GetBinaryChunkType();
	pFileStream->Write(&type, sizeof(type));

	SerializeBinaryChunkImpl(pFileStream);

	m_pParent = NULL;

	ClearCache();
}

void LeSerializableBinaryChunk::DeserializeBinaryChunk(LeStreamFile* pFileStream, LeSerializableBinaryChunk* pParent)
{
	LE_DEBUG_ASSERT(pFileStream);

	ClearCache();

	m_pParent = pParent;

	LeSerializableBinaryChunkType type;
	pFileStream->Read(&type, sizeof(type));
	LE_ASSERT(GetBinaryChunkType() == type);

	DeserializeBinaryChunkImpl(pFileStream);

	m_pParent = NULL;

	ClearCache();

}

LeSerializableBinaryChunk* LeSerializableBinaryChunk::FindParent(LeSerializableBinaryChunkType type)
{
	LeSerializableBinaryChunk* pScan = m_pParent;
	while (pScan)
	{
		if (pScan->GetBinaryChunkType() == type)
			return pScan;

		pScan = pScan->m_pParent;
	}

	return NULL;
}

LeSerializableBinaryChunk* LeSerializableBinaryChunk::FindParentCached(LeSerializableBinaryChunkType type)
{
	int32 i;
	for (i = 0; i < CACHE_SIZE; i++)
	{
		if (!m_Cache[i])
			break;
	
		if (m_Cache[i]->GetBinaryChunkType() == type)
			return m_Cache[i];
	}

	LeSerializableBinaryChunk* pFound = FindParent(type);
	if (pFound == NULL)
		return NULL;

	if (m_LastWrittenCacheIndex >= CACHE_SIZE)
		m_LastWrittenCacheIndex = 0;

	m_Cache[m_LastWrittenCacheIndex++] = pFound;

	return pFound;


}

void LeSerializableBinaryChunk::ClearCache(void)
{
	for (int32 i = 0; i < CACHE_SIZE; i++)
		m_Cache[i] = NULL;

	m_LastWrittenCacheIndex = 0;
}