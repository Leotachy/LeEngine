
#pragma once

#include "LeCore/LeMesh/LeMesh.h"

#include "LeCore/LeBinary/LeBinaryChunk.h"

class LeWorld : public LeBinaryChunk
{
public:
	LeWorld(void);
	~LeWorld(void);

	LeMesh* GetMesh() { return m_mesh; }

	void SerializeBinaryChunk(LeStreamFile* pFileStream) override;
	void DeserializeBinaryChunk(LeStreamFile* pFileStream) override;

protected:


private:

	LeMesh* m_mesh;
};