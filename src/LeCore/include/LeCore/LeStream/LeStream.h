
#pragma once

enum LeStreamType
{
	STREAM_TYPE_NONE,
	STREAM_TYPE_FILE,
};

enum LeStreamAccessType
{
	STREAM_ACCESS_NONE,
	STREAM_ACCESS_READ,
	STREAM_ACCESS_READ_TEXT,
	STREAM_ACCESS_WRITE
};



// base class
class LeStream
{
protected:
	LeStream();
public:
	virtual ~LeStream() = default;

	LeStreamType type;
	LeStreamAccessType accessType;
	uint32 position;

	// base functionality
	virtual LeStream* Open(LeStreamAccessType a, const void* data) = 0;
	virtual LeBool Close(void* data) = 0;
	virtual uint32 Read(void* buffer, uint32 length) = 0;
	virtual LeStream* Write(const void* buffer, uint32 length) = 0;
	virtual LeStream* Skip(uint32 offset) = 0;
	virtual LeStream* SkipBack(uint32 offset) = 0;

	LeStream* WriteString(const LeChar* str);

	// FIXME TODO THIS MESS IN TEMPLATES..........

	LeStream* WriteNumber(const LeBool* p, int32 num = 1);
	LeStream* WriteNumber(const uint8* p, int32 num = 1);
	LeStream* WriteNumber(const int8* p, int32 num = 1);
	LeStream* WriteNumber(const uint16* p, int32 num = 1);
	LeStream* WriteNumber(const int16* p, int32 num = 1);
	LeStream* WriteNumber(const uint32* p, int32 num = 1);
	LeStream* WriteNumber(const int32* p, int32 num = 1);
	LeStream* WriteNumber(const uint64* p, int32 num = 1);
	LeStream* WriteNumber(const int64* p, int32 num = 1);
	LeStream* WriteNumber(const float32* p, int32 num = 1);
	LeStream* WriteNumber(const float64* p, int32 num = 1);


	uint32 ReadString(LeChar* str, int32 buffer_len);

	uint32 ReadNumber(LeBool* p, int32 num = 1);
	uint32 ReadNumber(uint8* p, int32 num = 1);
	uint32 ReadNumber(int8* p, int32 num = 1);
	uint32 ReadNumber(uint16* p, int32 num = 1);
	uint32 ReadNumber(int16* p, int32 num = 1);
	uint32 ReadNumber(uint32* p, int32 num = 1);
	uint32 ReadNumber(int32* p, int32 num = 1);
	uint32 ReadNumber(uint64* p, int32 num = 1);
	uint32 ReadNumber(int64* p, int32 num = 1);
	uint32 ReadNumber(float32* p, int32 num = 1);
	uint32 ReadNumber(float64* p, int32 num = 1);
};