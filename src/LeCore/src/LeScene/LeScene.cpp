
#include "LeCore/lecore.h"

#include "LeCore/LeScene/LeScene.h"
#include "LeCore/LeRender/LeRender.h"

LeScene::LeScene()
{
	
}

void LeScene::SerializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_textureDictionary.SerializeBinaryChunk(pFileStream, this);
	m_materialDictionary.SerializeBinaryChunk(pFileStream, this);
	m_world.SerializeBinaryChunk(pFileStream, this);

}

void LeScene::DeserializeBinaryChunkImpl(LeStreamFile* pFileStream)
{
	m_textureDictionary.DeserializeBinaryChunk(pFileStream, this);
	m_materialDictionary.DeserializeBinaryChunk(pFileStream, this);
	m_world.DeserializeBinaryChunk(pFileStream, this);
}