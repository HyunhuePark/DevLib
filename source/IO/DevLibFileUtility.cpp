#include "../../include/IO/DevLibFileUtility.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#endif

namespace DevLib
{
	namespace IO
	{

		size_t ReadFolder(FileInfo &fileInfo, const std::string &path, bool bRecursive /*= false*/)
		{
			std::string pathMap = path;
#ifdef _MSC_VER
			if (pathMap.c_str()[pathMap.size() - 1] != '\\')
				pathMap += '\\';
			const std::string pathMapFilter = pathMap + '*';

			WIN32_FIND_DATAA fd;
			const HANDLE hFolder = FindFirstFileA(pathMapFilter.c_str(), &fd);

			if (INVALID_HANDLE_VALUE != hFolder)
			{
				do
				{
					if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (fd.cFileName[0] != '.')
						{
							fileInfo[pathMap + fd.cFileName] = true;

							if (bRecursive)
							{
								ReadFolder(fileInfo, pathMap + fd.cFileName, bRecursive);
							}
						}
					}
					else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					{
						fileInfo[pathMap + fd.cFileName] = false;
					}

				} while (FindNextFileA(hFolder, &fd));
			}

			FindClose(hFolder);

#elif defined(__linux__) // Linux
			if (pathMap.c_str()[pathMap.size() - 1] != '/')
			{
				pathMap += '/';
			}

			struct stat statinfo;
			DIR *dir = nullptr;
			struct dirent *readDir;

			if ((dir = opendir(pathMap.c_str())) != nullptr)
			{
				while ((readDir = readdir(dir)) != nullptr)
				{
					if (readDir->d_name[0] != '.')
					{
						std::string filePath = pathMap + readDir->d_name;

						lstat(filePath.c_str(), &statinfo);

						if (S_ISDIR(statinfo.st_mode))
						{
							fileInfo[filePath] = true;
							if (bRecursive)
							{
								ReadFolder(fileInfo, filePath, bRecursive);
							}
						}
						else
						{
							fileInfo[filePath] = false;
						}
					}
				}

				closedir(dir);
			}

#endif

			return fileInfo.size();
		}

		bool MakeDir(const std::string &path)
		{
			bool bRet = false;
			std::string _path = path;

			if (!_path.empty() && _path.size() < 2048)
			{
#ifdef _MSC_VER
				if (_path.c_str()[_path.size() - 1] != '\\')
				{
					_path += '\\';
				}
#elif defined(__linux__) // Linux
				if (_path.c_str()[_path.size() - 1] != '/')
				{
					_path += '/';
				}
#endif

				char tmp_path[2048] = "";
				uint64_t len = 0;
				const char *tmp = _path.c_str();

#ifdef _MSC_VER
				while ((tmp = strchr(tmp, '\\')) != nullptr)
#elif defined(__linux__) // Linux
				while ((tmp = strchr(tmp, '/')) != nullptr)
#endif
				{
					len = tmp - _path.c_str();
					tmp++;

					if (len == 0)
					{
						continue;
					}

#ifdef _MSC_VER
					strncpy_s(tmp_path, _path.c_str(), len);
					tmp_path[len] = 0x00;

					bRet = CreateDirectory(tmp_path, nullptr);

#elif defined(__linux__) // Linux
					strncpy(tmp_path, _path.c_str(), len);
					tmp_path[len] = 0x00;

					if (mkdir(tmp_path, 0777) == -1)
					{
						if (errno != EEXIST)
						{
							bRet = false;
							break;
						}
						else
						{
							bRet = true;
						}
					}
#endif

				}
			}
			return bRet;
		}

		bool RemoveFile(const std::string& file)
		{
			bool bRet = false;

			if( remove(file.c_str()) == 0 )
			{
				bRet = true;
			}

			return bRet;
		}

	}
}