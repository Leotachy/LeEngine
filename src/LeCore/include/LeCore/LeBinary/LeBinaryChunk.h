
#pragma once

#include "LeCore/LeStream/LeStreamFile.h"

class LeBinaryChunk
{
public:
	virtual void SerializeBinaryChunk(LeStreamFile* pFileStream) = 0;
	virtual void DeserializeBinaryChunk(LeStreamFile* pFileStream) = 0;
};