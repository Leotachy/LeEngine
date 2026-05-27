
#pragma once

enum class LeStreamType
{
	None,
	FILE,
};

enum class LeStreamAccessType
{
	None,
	READ,
	READ_TEXT,
	WRITE
};



// base class

class LeStream
{
protected:
	LeStream();
public:
	virtual ~LeStream() = default;

	LeStreamType Type;
	LeStreamAccessType AccessType;
	uint32 Position;

	// base functionality
	virtual LeStream* Open(LeStreamAccessType a, const void* data) = 0;
	virtual LeBool Close() = 0;
	virtual uint32 Read(void* buffer, uint32 length) = 0;
	virtual LeStream* Write(const void* buffer, uint32 length) = 0;
	virtual LeStream* Skip(uint32 offset) = 0;
	virtual LeStream* SkipBack(uint32 offset) = 0;

	LeStream* WriteString(const LeChar* str);
	uint32 ReadString(LeChar* str, int32 buffer_len);


	template<typename T>
	LeStream* WriteNumber(const T* p, int32 num = 1)
	{
		return Write((void*)p, num * sizeof(T));
	}
	
	template<typename T>
	uint32 ReadNumber(T* p, int32 num = 1)
	{
		return Read((void*)p, num * sizeof(T));
	}
};