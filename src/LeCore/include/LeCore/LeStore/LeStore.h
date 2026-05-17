
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
	static void ForEachFileInDirectory(const LeChar* pSearchPattern, Func&& func)
	{
		LE_DEBUG_ASSERT(pSearchPattern);

		std::vector<const LeChar*> Files;
		GetFilesInDirectory(Files, pSearchPattern);

		for (const LeChar* file : Files)
		{
			func(file);
			delete file;
		}
	}

	template<typename Func>
	static void ForEachDirectoryInDirectory(const LeChar* pSearchPattern, Func&& func)
	{
		LE_DEBUG_ASSERT(pSearchPattern);

		std::vector<const LeChar*> Directories;
		GetDirectoriesInDirectory(Directories, pSearchPattern);

		for (const LeChar* directory : Directories)
		{
			func(directory);
			delete directory;
		}
	}

private:
	// Platform specific
	static void GetFilesInDirectory(std::vector<const LeChar*>& out, const LeChar* pSearchPattern);

	static void GetDirectoriesInDirectory(std::vector<const LeChar*>& out, const LeChar* pSearchPattern);
	
};