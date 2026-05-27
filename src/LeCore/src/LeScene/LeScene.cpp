
#include "LeCore/lecore.h"

#include "LeCore/LeScene/LeScene.h"
#include "LeCore/LeRender/LeRender.h"

LeScene::LeScene()
{
	
}

void LeScene::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_TextureDictionary.SerializeBinaryChunk(pFileStream, this);
	m_MaterialDictionary.SerializeBinaryChunk(pFileStream, this);
	m_World.SerializeBinaryChunk(pFileStream, this);

}

void LeScene::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_TextureDictionary.DeserializeBinaryChunk(pFileStream, this);
	m_MaterialDictionary.DeserializeBinaryChunk(pFileStream, this);
	m_World.DeserializeBinaryChunk(pFileStream, this);
}