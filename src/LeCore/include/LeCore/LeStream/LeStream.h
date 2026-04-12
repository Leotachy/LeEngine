
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
	virtual LeStream* Open(LeStreamType t, LeStreamAccessType a, void* data) = 0;
	virtual LeBool Close(void* data) = 0;
	virtual LeStream* Read(void* buffer, uint32 length) = 0;
	virtual LeStream* Write(const void* buffer, uint32 length) = 0;
	virtual LeStream* Skip(uint32 offset) = 0;
	virtual LeStream* SkipBack(uint32 offset) = 0;

	LeStream* WriteString(const LeChar* str);

	LeStream* WriteNumber(LeBool* p, int32 num = 1);
	LeStream* WriteNumber(uint8* p, int32 num = 1);
	LeStream* WriteNumber(int8* p, int32 num = 1);
	LeStream* WriteNumber(uint16* p, int32 num = 1);
	LeStream* WriteNumber(int16* p, int32 num = 1);
	LeStream* WriteNumber(uint32* p, int32 num = 1);
	LeStream* WriteNumber(int32* p, int32 num = 1);
	LeStream* WriteNumber(uint64* p, int32 num = 1);
	LeStream* WriteNumber(int64* p, int32 num = 1);
	LeStream* WriteNumber(float32* p, int32 num = 1);
	LeStream* WriteNumber(float64* p, int32 num = 1);


	LeStream* ReadString(LeChar* str, int32 buffer_len);

	LeStream* ReadNumber(LeBool* p, int32 num = 1);
	LeStream* ReadNumber(uint8* p, int32 num = 1);
	LeStream* ReadNumber(int8* p, int32 num = 1);
	LeStream* ReadNumber(uint16* p, int32 num = 1);
	LeStream* ReadNumber(int16* p, int32 num = 1);
	LeStream* ReadNumber(uint32* p, int32 num = 1);
	LeStream* ReadNumber(int32* p, int32 num = 1);
	LeStream* ReadNumber(uint64* p, int32 num = 1);
	LeStream* ReadNumber(int64* p, int32 num = 1);
	LeStream* ReadNumber(float32* p, int32 num = 1);
	LeStream* ReadNumber(float64* p, int32 num = 1);
};