#pragma once

#include "../Base/DevLibTypes.hpp"
#include <sstream>
#include <iomanip>
#include <vector>

namespace DevLib {

	std::wstring UTF8ToUnicode(const std::string& str);
	std::string UTF8ToMultiByte(const std::string& str);
	std::string UnicodeToMultiByte(const std::wstring& str);
	std::string UnicodeToUTF8(const std::wstring& str);
	std::wstring MultiByteToUnicode(const std::string& str);
	std::string MultiByteToUTF8(const std::string& str);

	template< typename  Type>
	string_t to_string_fill(const Type& value, const char_t elem, const uint32_t width)
	{
		std::ostringstream oss;
		oss << std::setfill(elem) << std::setw(width) << value;

		return std::move(oss.str());
	}

	inline std::vector<std::string> Split(const std::string& inputString, char delimiter )
	{
		std::vector<std::string> vString;
		std::stringstream ss(inputString);
		std::string temp;

		while (getline(ss, temp, delimiter))
		{
			vString.push_back(temp);
		}

		return vString;
	}

	inline bool IsNumber(const std::string& str)
	{
		bool bRet = true;
		for (char const &c : str)
		{
			if (std::isdigit(c) == 0)
			{
				bRet = false;
				break;
			}
		}
		return bRet;
	}

}
