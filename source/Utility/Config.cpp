#include "../../include/Utility/Config.hpp"

#include <vector>
#include <string>

#include <cjson/cJSON.h>
#include "../../include/IO/CFile.hpp"
#include <cstring>

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <unistd.h>
#endif

namespace DevLib
{
	string_t _devlib_process_path_ = {};
	string_t _devlib_process_name_ = {};

	string_t GetProcessName()
	{
		if (_devlib_process_path_.empty())
		{
#if defined(_MSC_VER)
			char processName[8096] = { 0, };
			GetModuleFileNameA(nullptr, processName, 8096);

			_devlib_process_path_ = string_t(processName);

			const size_t start = _devlib_process_path_.rfind('\\');
			const size_t end = _devlib_process_path_.rfind('.');
			const size_t size = end - start;

			if (size >= 1)
			{
				_devlib_process_name_ = _devlib_process_path_.substr(start + 1, end - (start + 1));
			}
#elif defined(__linux__) // Linux
			char_t fname[1024] = "";
			char_t process_path[1024] = "";
			if (sprintf(fname, "/proc/%d/cmdline", getpid()) > 0)
			{
				FILE* f = fopen(fname, "r");
				if (f != 0)
				{
					size_t size;
					size = fread(process_path, sizeof(int8_t), 1024, f);
					if (size > 0)
					{
						_devlib_process_name_ = string_t(process_path);
					}

				}
			}
#endif

		}

		return _devlib_process_name_;
	}

	string_t GetProcessPath()
	{
		if (_devlib_process_path_.empty())
		{
			GetProcessName();
		}

		return _devlib_process_path_;

	}

	string_t GetMakeConfigName()
	{
		return GetProcessName() + string_t(".json");
	}

	int32_t GetConfigInt(const string_t& GroupName, const string_t& Identity, int32_t defValue,
		const string_t& configName)
	{
		int32_t rVal = defValue;

		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();

			// Json 
			cJSON* json = cJSON_Parse(buff.data());
			if (json == nullptr)
			{
				SetConfigInt(GroupName, Identity, defValue, configName);
			}
			else
			{
				const cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

				if (itemGroup != nullptr)
				{
					const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

					if (valueItem != nullptr)
					{
						rVal = valueItem->valueint;
					}
					else
					{
						SetConfigInt(GroupName, Identity, defValue, configName);
					}
				}
				else
				{
					SetConfigInt(GroupName, Identity, defValue, configName);
				}

				cJSON_Delete(json);
			}

		}
		else
		{
			SetConfigInt(GroupName, Identity, defValue, configName);
		}


		return rVal;
	}

	string_t GetConfigString(const string_t& GroupName, const string_t& Identity, const string_t& defValue,
		const string_t& configName)
	{
		string_t retValue = defValue;

		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();

			// Json 
			cJSON* json = cJSON_Parse(buff.data());
			if (json == nullptr)
			{
				SetConfigString(GroupName, Identity, defValue, configName);
			}
			else
			{
				const cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

				if (itemGroup != nullptr)
				{
					const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

					if (valueItem != nullptr)
					{
						retValue = string_t(valueItem->valuestring);
					}
					else
					{
						SetConfigString(GroupName, Identity, defValue, configName);
					}
				}
				else
				{
					SetConfigString(GroupName, Identity, defValue, configName);
				}

				cJSON_Delete(json);
			}

		}
		else
		{
			SetConfigString(GroupName, Identity, defValue, configName);
		}
		return retValue;
	}

	double_t GetConfigDouble(const string_t& GroupName, const string_t& Identity, double_t defValue,
		const string_t& configName)
	{
		double_t rVal = defValue;

		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();

			// Json 
			cJSON* json = cJSON_Parse(buff.data());
			if (json == nullptr)
			{
				SetConfigDouble(GroupName, Identity, defValue, configName);
			}
			else
			{
				const cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

				if (itemGroup != nullptr)
				{
					const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

					if (valueItem != nullptr)
					{
						rVal = valueItem->valuedouble;
					}
					else
					{
						SetConfigDouble(GroupName, Identity, defValue, configName);
					}
				}
				else
				{
					SetConfigDouble(GroupName, Identity, defValue, configName);
				}

				cJSON_Delete(json);
			}
		}
		else
		{
			SetConfigDouble(GroupName, Identity, defValue, configName);
		}


		return rVal;
	}

	void SetConfigInt(const string_t& GroupName, const string_t& Identity, int32_t value,
		const string_t& configName)
	{
		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();
			(void)file.Open(configName, "wt");
		}
		else
		{
			(void)file.Open(configName, "wt");
			buff.resize(1024);
		}


		// Json 
		cJSON* json = cJSON_Parse(buff.data());
		if (json == nullptr)
		{
			json = cJSON_CreateObject();
			cJSON* itemGroup = cJSON_CreateObject();
			cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup);

			cJSON* itemValue = cJSON_CreateNumber(value);
			cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
		}
		else
		{
			cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

			if (itemGroup != nullptr)
			{
				const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

				cJSON* itemValue = cJSON_CreateNumber(value);
				if (valueItem != nullptr)
				{
					cJSON_ReplaceItemInObject(itemGroup, Identity.c_str(), itemValue);
				}
				else
				{
					cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
				}
			}
			else
			{
				cJSON* itemGroup2 = cJSON_CreateObject();
				cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup2);

				cJSON* itemValue = cJSON_CreateNumber(value);
				cJSON_AddItemToObject(itemGroup2, Identity.c_str(), itemValue);
			}
		}

		char* out = cJSON_Print(json);
		(void)file.Write(out, strlen(out));
		free(out);
		file.Flush();
		file.Close();
		cJSON_Delete(json);
	}

	void SetConfigString(const string_t& GroupName, const string_t& Identity, const string_t& value,
		const string_t& configName)
	{
		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();
			(void)file.Open(configName, "wt");
		}
		else
		{
			(void)file.Open(configName, "wt");
			buff.resize(1024);
		}


		// Json 
		cJSON* json = cJSON_Parse(buff.data());
		if (json == nullptr)
		{
			json = cJSON_CreateObject();
			cJSON* itemGroup = cJSON_CreateObject();
			cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup);

			cJSON* itemValue = cJSON_CreateString(value.c_str());
			cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
		}
		else
		{
			cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

			if (itemGroup != nullptr)
			{
				const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

				cJSON* itemValue = cJSON_CreateString(value.c_str());
				if (valueItem != nullptr)
				{
					cJSON_ReplaceItemInObject(itemGroup, Identity.c_str(), itemValue);
				}
				else
				{
					cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
				}
			}
			else
			{
				cJSON* itemGroup2 = cJSON_CreateObject();
				cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup2);

				cJSON* itemValue = cJSON_CreateString(value.c_str());
				cJSON_AddItemToObject(itemGroup2, Identity.c_str(), itemValue);
			}
		}

		char* out = cJSON_Print(json);
		(void)file.Write(out, strlen(out));
		free(out);
		file.Flush();
		file.Close();
		cJSON_Delete(json);
	}

	void SetConfigDouble(const string_t& GroupName, const string_t& Identity, double_t value,
		const string_t& configName)
	{
		IO::CFile file;
		std::vector<char> buff;

		if (file.Open(configName, "rt"))
		{
			buff.resize(static_cast<uint32_t>(file.GetFileSize() * 2));
			(void)file.Read(buff.data(), buff.size());

			file.Close();
			(void)file.Open(configName, "wt");
		}
		else
		{
			(void)file.Open(configName, "wt");
			buff.resize(1024);
		}


		// Json 
		cJSON* json = cJSON_Parse(buff.data());
		if (json == nullptr)
		{
			json = cJSON_CreateObject();
			cJSON* itemGroup = cJSON_CreateObject();
			cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup);

			cJSON* itemValue = cJSON_CreateNumber(value);
			cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
		}
		else
		{
			cJSON* itemGroup = cJSON_GetObjectItem(json, GroupName.c_str());

			if (itemGroup != nullptr)
			{
				const cJSON* valueItem = cJSON_GetObjectItem(itemGroup, Identity.c_str());

				cJSON* itemValue = cJSON_CreateNumber(value);
				if (valueItem != nullptr)
				{
					cJSON_ReplaceItemInObject(itemGroup, Identity.c_str(), itemValue);
				}
				else
				{
					cJSON_AddItemToObject(itemGroup, Identity.c_str(), itemValue);
				}
			}
			else
			{
				cJSON* itemGroup2 = cJSON_CreateObject();
				cJSON_AddItemToObject(json, GroupName.c_str(), itemGroup2);

				cJSON* itemValue = cJSON_CreateNumber(value);
				cJSON_AddItemToObject(itemGroup2, Identity.c_str(), itemValue);
			}
		}

		char *out = cJSON_Print(json);
		(void)file.Write(out, strlen(out));
		free(out);
		file.Flush();
		file.Close();
		cJSON_Delete(json);
	}
}
