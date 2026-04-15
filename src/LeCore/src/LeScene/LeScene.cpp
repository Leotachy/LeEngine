
#include "LeCore/lecore.h"

#include "LeCore/LeScene/LeScene.h"

LeScene::LeScene()
{

}

void LeScene::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_textureDictionary.SerializeBinaryChunk(pFileStream);
	m_materialDictionary.SerializeBinaryChunk(pFileStream);
	m_world.SerializeBinaryChunk(pFileStream);

}

void LeScene::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_textureDictionary.DeserializeBinaryChunk(pFileStream);
	m_materialDictionary.DeserializeBinaryChunk(pFileStream);
	m_world.DeserializeBinaryChunk(pFileStream);
}