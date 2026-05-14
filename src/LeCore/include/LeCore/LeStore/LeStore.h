
#pragma once

// Storage utility. 

class LeStore
{
private:
	// static class
	LeStore() = delete;
	~LeStore() = delete;
public:

	// util
	static LeBool HasFileExtension(const LeChar* pFilename, const LeChar* pExtension);
	static LeBool DropExtension(LeChar* pBuf, const LeChar* pFilename, const LeChar* pExtension);

	template<typename Func>
	static void ForEachFileInDirectoryWithExtension(const LeChar* pDir, const LeChar* pExtension, Func&& func)
	{
		LE_DEBUG_ASSERT(pDir);

		if (!pExtension)
		{
			ForEachFileInDirectory(pDir, func);
			return;
		}

		ForEachFileInDirectory(pDir, [pExtension, &func](const LeChar* pFileName)
			{

				if (HasFileExtension(pFileName, pExtension))
				{
					func(pFileName);
				}
			});
	}

	template<typename Func>
	static void ForEachFileInDirectory(const LeChar* pDir, Func&& func)
	{
		LE_DEBUG_ASSERT(pDir);

		std::vector<const LeChar*> Files;
		GetFilesInDirectory(Files, pDir);

		for (const LeChar* file : Files)
		{
			func(file);
			delete file;
		}
	}

	template<typename Func>
	static void ForEachDirectoryInDirectory(const LeChar* pDir, Func&& func)
	{
		LE_DEBUG_ASSERT(pDir);

		std::vector<const LeChar*> Directories;
		GetDirectoriesInDirectory(Directories, pDir);

		for (const LeChar* directory : Directories)
		{
			func(directory);
			delete directory;
		}
	}

private:
	// Platform specific
	static void GetFilesInDirectory(std::vector<const LeChar*>& out, const LeChar* pDir);

	static void GetDirectoriesInDirectory(std::vector<const LeChar*>& out, const LeChar* pDir);
	
};