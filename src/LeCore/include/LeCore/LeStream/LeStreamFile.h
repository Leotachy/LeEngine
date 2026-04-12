
#pragma once

#include "LeCore/LeStream/LeStream.h"

#define MAX_LE_FILENAME 128

class LeStreamFile : public LeStream
{
public:
	LeStreamFile(void);

	LeStream* Open(LeStreamAccessType a, const void* data) override;
	LeBool Close(void* data) override;
	uint32 Read(void* buffer, uint32 length) override;
	LeStream* Write(const void* buffer, uint32 length) override;
	LeStream* Skip(uint32 offset) override;
	LeStream* SkipBack(uint32 offset) override;

protected:

	LeChar filename[MAX_LE_FILENAME];

	FILE* m_file;

};