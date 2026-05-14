
#include <stdio.h>
#include <io.h>

#include "LeCore/LeCore.h"

#include "LeCore/LeStore/LeStore.h"

#ifdef _WIN32

// Platform specific
void LeStore::GetFilesInDirectory(std::vector<const LeChar*>& out, const LeChar* pDir)
{
	LE_DEBUG_ASSERT(pDir);

	_finddata_t fd;

	intptr_t handle = _findfirst(pDir, &fd);

	if (handle != -1)
	{
		int32 ret = 0;

		while (ret == 0)
		{
			out.push_back(_strdup(fd.name));

			ret = _findnext(handle, &fd);
		}
	}
	_findclose(handle);
}

void LeStore::GetDirectoriesInDirectory(std::vector<const LeChar*>& out, const LeChar* pDir)
{
	LE_DEBUG_ASSERT(pDir);

	_finddata_t fd;

	intptr_t handle = _findfirst(pDir, &fd);

	if (handle != -1)
	{
		int32 ret = 0;

		while (ret == 0)
		{
			if ((fd.attrib & _A_SUBDIR) && !(fd.attrib & _A_ARCH))
			{
				out.push_back(_strdup(fd.name));
			}

			ret = _findnext(handle, &fd);
		}
	}
	_findclose(handle);
}
#endif
