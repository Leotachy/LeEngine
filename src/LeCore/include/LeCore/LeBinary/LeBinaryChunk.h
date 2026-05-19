
#pragma once

#include "LeCore/LeStream/LeStreamFile.h"

class LeBinaryChunk
{
public:
	virtual void SerializeBinaryChunk(LeStreamFile* pFileStream) {};
	virtual void DeserializeBinaryChunk(LeStreamFile* pFileStream) {};
};