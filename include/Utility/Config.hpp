#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib {

	string_t GetProcessName();
	string_t GetProcessPath();
	string_t GetMakeConfigName();

	int32_t GetConfigInt(const string_t& GroupName, const string_t& Identity, int32_t defValue, const string_t& configName = GetMakeConfigName());
	string_t GetConfigString(const string_t& GroupName, const string_t& Identity, const string_t& defValue, const string_t& configName = GetMakeConfigName());
	double_t GetConfigDouble(const string_t& GroupName, const string_t& Identity, double_t defValue, const string_t& configName = GetMakeConfigName());

	void SetConfigInt(const string_t& GroupName, const string_t& Identity, int32_t value, const string_t& configName = GetMakeConfigName());
	void SetConfigString(const string_t& GroupName, const string_t& Identity, const string_t& value, const string_t& configName = GetMakeConfigName());
	void SetConfigDouble(const string_t& GroupName, const string_t& Identity, double_t value, const string_t& configName = GetMakeConfigName());

}
