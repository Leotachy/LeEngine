
#include "LeCore/lecore.h"

#include "LeCore/LeStream/LeStreamFile.h"

LeStreamFile::LeStreamFile(void)
{
	Type = LeStreamType::FILE;

	LeMemset(m_Filename, 0, sizeof(m_Filename));
	m_pFile = NULL;
}

LeStream* LeStreamFile::Open(LeStreamAccessType a, const void* data)
{
	LE_DEBUG_ASSERT(data);

	AccessType = a;

	Position = 0; 

	strncpy(m_Filename, (const char*)data, MAX_LE_FILENAME-1);
	m_Filename[MAX_LE_FILENAME - 1] = 0;


	switch (AccessType)
	{
	case LeStreamAccessType::READ:
		m_pFile = fopen(m_Filename, "rb");
		break;

	case LeStreamAccessType::READ_TEXT:
		m_pFile = fopen(m_Filename, "rt");
		break;

	case LeStreamAccessType::WRITE:
		m_pFile = fopen(m_Filename, "wb");
		break;

	default:
		LE_DEBUG_ASSERT(0);
		m_pFile = NULL;
		break;
	}

	LE_DEBUG_ASSERT(m_pFile);

	if (m_pFile == NULL)
	{
		return NULL;
	}
	else
	{
		return this;
	}
}

LeBool LeStreamFile::Close()
{
	LE_DEBUG_ASSERT(m_pFile);

	int code = fclose(m_pFile);
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
	LE_DEBUG_ASSERT(AccessType == LeStreamAccessType::READ || AccessType == LeStreamAccessType::READ_TEXT);
	LE_DEBUG_ASSERT(buffer);
	LE_DEBUG_ASSERT(length > 0);

	uint32 BytesRead = 0;

	LE_DEBUG_ASSERT(m_pFile);

	BytesRead = int32(fread(buffer, sizeof(uint8), length, m_pFile));

	Position += BytesRead;

	if (BytesRead < length)
	{
		if (feof(m_pFile))
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

	if (AccessType == LeStreamAccessType::WRITE)
	{
		LE_DEBUG_ASSERT(m_pFile);

		BytesWritten = int32(fwrite(buffer, sizeof(uint8), length, m_pFile));

		Position += BytesWritten;

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

	LE_DEBUG_ASSERT(m_pFile);

	code = fseek(m_pFile, offset, SEEK_CUR);
	LE_DEBUG_ASSERT(code == 0);

	Position += offset;

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
	LE_DEBUG_ASSERT(AccessType == LeStreamAccessType::WRITE);
	LE_DEBUG_ASSERT(m_pFile);

	LeStream* ret = NULL;

	int32 code = fseek(m_pFile, -(static_cast<int32> (offset)), SEEK_CUR);
		
	LE_DEBUG_ASSERT(code == 0);

	Position -= offset;

	if (code == 0)
	{
		ret = this;
	}

	return ret;
}