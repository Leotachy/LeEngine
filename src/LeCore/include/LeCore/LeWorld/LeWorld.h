
#pragma once

#include "LeCore/LeMesh/LeMesh.h"

#include "LeCore/LeBinary/LeSerializableBinaryChunk.h"

class LeWorld : public LeSerializableBinaryChunk
{
public:
	LeWorld(void);
	~LeWorld(void);

	LeMesh* GetMesh() { return m_pMesh; }

	LeSerializableBinaryChunkType GetBinaryChunkType(void) { return LeSerializableBinaryChunkType::World; }
	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

protected:


private:

	LeMesh* m_pMesh;
};