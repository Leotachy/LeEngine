
#include "LeCore/LeCore.h"

#include "LeCore/LeStream/LeStream.h"

LeStream::LeStream()
{
	Type = LeStreamType::None;
	AccessType = LeStreamAccessType::None;
	Position = 0;
}


LeStream* LeStream::WriteString(const LeChar* str)
{

	if (!str)
	{
		int32 zero = 0;
		if (!WriteNumber(&zero))
			return NULL;
		return this;
	}
		
	int32 str_len = int32(strlen(str));

	if (!WriteNumber(&str_len)) return NULL;

	if (str_len)
	{
		//** Note SpUInt8 is unsigned char, same as SpChar ..
		if (!WriteNumber((uint8*)str, str_len)) return NULL;
	}

	return this;
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