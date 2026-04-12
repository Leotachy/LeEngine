
#include "LeCore/lecore.h"

#include "LeCore/LeStream/LeStreamFile.h"

LeStreamFile::LeStreamFile(void)
{
	type = LeStreamType::STREAM_TYPE_FILE;

	LeMemset(filename, 0, sizeof(filename));
	m_file = NULL;
}

LeStream* LeStreamFile::Open(LeStreamAccessType a, const void* data)
{
	LE_DEBUG_ASSERT(data);

	accessType = a;

	position = 0; 

	strncpy(filename, (const char*)data, MAX_LE_FILENAME-1);
	filename[MAX_LE_FILENAME - 1] = 0;


	switch (accessType)
	{
	case LeStreamAccessType::STREAM_ACCESS_READ:
		m_file = fopen(filename, "rb");
		break;

	case LeStreamAccessType::STREAM_ACCESS_READ_TEXT:
		m_file = fopen(filename, "rt");
		break;

	case LeStreamAccessType::STREAM_ACCESS_WRITE:
		m_file = fopen(filename, "wb");
		break;

	default:
		LE_DEBUG_ASSERT(0);
		m_file = NULL;
		break;
	}

	LE_DEBUG_ASSERT(m_file);

	if (m_file == NULL)
	{
		return NULL;
	}
	else
	{
		return this;
	}
}

LeBool LeStreamFile::Close(void* data)
{
	LE_DEBUG_ASSERT(m_file);

	int code = fclose(m_file);
	LE_DEBUG_ASSERT(code == 0);

	if (code != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

uint32 LeStreamFile::Read(void* buffer, uint32 length)
{
	LE_DEBUG_ASSERT(accessType == LeStreamAccessType::STREAM_ACCESS_READ || accessType == LeStreamAccessType::STREAM_ACCESS_READ_TEXT);
	LE_DEBUG_ASSERT(buffer);
	LE_DEBUG_ASSERT(length > 0);

	uint32 BytesRead = 0;

	LE_DEBUG_ASSERT(m_file);

	BytesRead = int32(fread(buffer, sizeof(uint8), length, m_file));

	position += BytesRead;

	if (BytesRead < length)
	{
		if (feof(m_file))
		{
			//LE_DEBUG_ERROR("Unexpected end of file!");
		}
	}

	return BytesRead;
}

LeStream* LeStreamFile::Write(const void* buffer, uint32 length)
{
	uint32 BytesWritten;

	LE_DEBUG_ASSERT(buffer);
	LE_DEBUG_ASSERT(length > 0);

	LeStream* ret;

	if (accessType == LeStreamAccessType::STREAM_ACCESS_WRITE)
	{
		LE_DEBUG_ASSERT(m_file);

		BytesWritten = int32(fwrite(buffer, sizeof(uint8), length, m_file));

		position += BytesWritten;

		if (BytesWritten < length)
		{
			ret = NULL;
		}
		else
		{
			ret = this;
		}
	}
	else
	{
		ret = NULL;
	}

	LE_DEBUG_ASSERT(ret);
	return ret;
}

LeStream* LeStreamFile::Skip(uint32 offset)
{
	int32 code;

	LE_DEBUG_ASSERT(m_file);

	code = fseek(m_file, offset, SEEK_CUR);
	LE_DEBUG_ASSERT(code == 0);

	position += offset;

	if (code == 0)
	{
		return this;
	}
	else
	{
		return NULL;
	}
}

LeStream* LeStreamFile::SkipBack(uint32 offset)
{
	LE_DEBUG_ASSERT(accessType == LeStreamAccessType::STREAM_ACCESS_WRITE);
	LE_DEBUG_ASSERT(m_file);

	LeStream* ret = NULL;

	int32 code = fseek(m_file, -(static_cast<int32> (offset)), SEEK_CUR);
		
	LE_DEBUG_ASSERT(code == 0);

	position -= offset;

	if (code == 0)
	{
		ret = this;
	}

	return ret;
}