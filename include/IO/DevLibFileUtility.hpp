#pragma once

#include "../Base/DevLibTypes.hpp"
#include <map>

namespace DevLib
{
	namespace IO
	{
		using FileInfo = std::map<std::string, bool /*true : Folder, false : File*/>;

		size_t ReadFolder(FileInfo& fileInfo, const std::string& path, bool bRecursive = false);

		bool MakeDir(const std::string& path);

		bool RemoveFile(const std::string& file);
	}
}