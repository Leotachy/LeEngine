
#pragma once

#include "LeCore/LeStream/LeStream.h"

#define MAX_LE_FILENAME 128

class LeStreamFile : public LeStream
{
public:
	LeStreamFile(void);

	LeStream* Open(LeStreamAccessType a, void* data);
	LeBool Close(void* data);
	LeStream* Read(void* buffer, uint32 length);
	LeStream* Write(const void* buffer, uint32 length);
	LeStream* Skip(uint32 offset);
	LeStream* SkipBack(uint32 offset);

protected:

	LeChar filename[MAX_LE_FILENAME];

	FILE* m_file;

};