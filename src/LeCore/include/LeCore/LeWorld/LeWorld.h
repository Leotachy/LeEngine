
#pragma once

#include "LeCore/LeMesh/LeMesh.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeWorld : public LeBinaryChunk
{
public:
	LeWorld(void);
	~LeWorld(void);

	LeMesh* GetMesh() { return m_mesh; }

protected:

	void SerializeBinaryChunkImpl(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunkImpl(LeStreamFile* pFileStream) override;

private:

	LeMesh* m_mesh;
};