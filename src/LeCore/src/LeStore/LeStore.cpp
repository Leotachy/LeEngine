
#include "LeCore/LeCore.h"

#include "LeCore/LeStore/LeStore.h"

LeBool LeStore::HasFileExtension(const LeChar* pFilename, const LeChar* pExtension)
{
	LE_DEBUG_ASSERT(pFilename);

	const LeChar* pExtensionChar = strrchr(pFilename, '.');
	if (!pExtensionChar)
	{
		if (pExtension)
			return false;
		return true;
	}
		
	if (!pExtension)
		return false;

	const LeChar* pFoundExtension = pExtensionChar;

	return strcmp(pFoundExtension, pExtension) == 0;
}

LeBool LeStore::DropExtension(LeChar* pBuf, const LeChar* pFilename, const LeChar* pExtension)
{
	LE_DEBUG_ASSERT(pBuf);
	LE_DEBUG_ASSERT(pFilename);
	LE_DEBUG_ASSERT(pExtension);
	
	const LeChar* pNextFind = NULL;
	const LeChar* pFoundExtension;
	do
	{
		pFoundExtension = pNextFind;
		pNextFind = strstr(pFilename, pExtension);
	} while (pNextFind);

	if (!pFoundExtension)
		return false;

	int64 strLen = pFoundExtension - pFilename;
	strncpy(pBuf, pFilename, strLen);
	pBuf[strLen] = 0;

	return true;
}