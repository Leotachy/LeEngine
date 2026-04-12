
#include "LeCore/LeCore.h"

#include "LeCore/LeStream/LeStream.h"

LeStream::LeStream()
{
	type = LeStreamType::STREAM_TYPE_NONE;
	accessType = LeStreamAccessType::STREAM_ACCESS_NONE;
	position = 0;
}


LeStream* LeStream::WriteString(const LeChar* str)
{
	LE_ASSERT(str);

	int32 str_len = int32(strlen(str));

	if (!WriteNumber(&str_len)) return NULL;

	if (str_len)
	{
		//** Note SpUInt8 is unsigned char, same as SpChar ..
		if (!WriteNumber((uint8*)str, str_len)) return NULL;
	}

	return this;
}


// raw read / write functions

LeStream* LeStream::WriteNumber(LeBool* p, int32 num)
{
	return Write((void*)p, num * sizeof(LeBool));
}

LeStream* LeStream::WriteNumber(uint8* p, int32 num)
{
	return Write((void*)p, num * sizeof(uint8));
}

LeStream* LeStream::WriteNumber(int8* p, int32 num)
{
	return Write((void*)p, num * sizeof(int8));
}

LeStream* LeStream::WriteNumber(uint16* p, int32 num)
{
	return Write((void*)p, num * sizeof(uint16));
}

LeStream* LeStream::WriteNumber(int16* p, int32 num)
{
	return Write((void*)p, num * sizeof(int16));
}

LeStream* LeStream::WriteNumber(uint32* p, int32 num)
{
	return Write((void*)p, num * sizeof(uint32));
}

LeStream* LeStream::WriteNumber(int32* p, int32 num)
{
	return Write((void*)p, num * sizeof(int32));
}

LeStream* LeStream::WriteNumber(uint64* p, int32 num)
{
	return Write((void*)p, num * sizeof(uint64));
}

LeStream* LeStream::WriteNumber(int64* p, int32 num)
{
	return Write((void*)p, num * sizeof(int64));
}

LeStream* LeStream::WriteNumber(float32* p, int32 num)
{
	return Write((void*)p, num * sizeof(float32));
}

LeStream* LeStream::WriteNumber(float64* p, int32 num)
{
	return Write((void*)p, num * sizeof(float64));
}


uint32 LeStream::ReadString(LeChar* str, int32 buffer_len)
{

	LE_ASSERT(str);
	LE_ASSERT(buffer_len > 0);

	int32 stored_len = 0, read_len;
	int32 str_len = buffer_len - 1;

	if (!ReadNumber(&stored_len)) return NULL;

	if (stored_len > str_len)
		read_len = str_len;
	else
		read_len = stored_len;

	if (read_len > 0)
		if (!ReadNumber((uint8*)str, read_len)) return NULL;

	if (read_len >= 0)
		str[read_len] = 0; // Terminate the string

	// skip unread chars
	int32 skip_len = stored_len - read_len;
	if (skip_len)
		Skip(skip_len);

	return read_len + skip_len + sizeof(stored_len);
}

uint32 LeStream::ReadNumber(LeBool* p, int32 num)
{
	return Read((void*)p, num * sizeof(LeBool));
}

uint32 LeStream::ReadNumber(uint8* p, int32 num)
{
	return Read((void*)p, num * sizeof(uint8));
}

uint32 LeStream::ReadNumber(int8* p, int32 num)
{
	return Read((void*)p, num * sizeof(int8));
}

uint32 LeStream::ReadNumber(uint16* p, int32 num)
{
	return Read((void*)p, num * sizeof(uint16));
}

uint32 LeStream::ReadNumber(int16* p, int32 num)
{
	return Read((void*)p, num * sizeof(int16));
}

uint32 LeStream::ReadNumber(uint32* p, int32 num)
{
	return Read((void*)p, num * sizeof(uint32));
}

uint32 LeStream::ReadNumber(int32* p, int32 num)
{
	return Read((void*)p, num * sizeof(int32));
}

uint32 LeStream::ReadNumber(uint64* p, int32 num)
{
	return Read((void*)p, num * sizeof(uint64));
}

uint32 LeStream::ReadNumber(int64* p, int32 num)
{
	return Read((void*)p, num * sizeof(int64));
}

uint32 LeStream::ReadNumber(float32* p, int32 num)
{
	return Read((void*)p, num * sizeof(float32));
}

uint32 LeStream::ReadNumber(float64* p, int32 num)
{
	return Read((void*)p, num * sizeof(float64));
}