
#include "LeCore/LeCore.h"

#include "LeCore/LeStore/LeStore.h"

LeBool LeStore::HasFileExtension(const LeChar* pFilename, const LeChar* pExtension)
{
	LE_DEBUG_ASSERT(pFilename);

	LeChar buf[256];
	if (GetFileExtension(buf, pFilename))
	{
		return strcmp(buf, pExtension) == 0;
	}

	if (!pExtension) // the file has no extension and no extension was provided
		return true;

	// file has no extension and extension was provided
	return false;

	
}

LeBool LeStore::GetFileExtension(LeChar* pBuf, const LeChar* pFilename)
{
	const LeChar* pExtensionChar = strrchr(pFilename, '.');
	if (pExtensionChar)
	{
		strcpy(pBuf, pExtensionChar);
		return true;
	}

	return false;
}

LeBool LeStore::DropExtension(LeChar* pBuf, const LeChar* pFilename, const LeChar* pExtension)
{
	LE_DEBUG_ASSERT(pBuf);
	LE_DEBUG_ASSERT(pFilename);
	LE_DEBUG_ASSERT(pExtension);
	

	const LeChar* pNextFind = strstr(pFilename, pExtension);
	const LeChar* pFoundExtension = pNextFind;

	if (!pFoundExtension)
		return false;

	int64 strLen = pFoundExtension - pFilename;
	strncpy(pBuf, pFilename, strLen);
	pBuf[strLen] = 0;

	return true;
}