#include "../../include/Utility/String.hpp"

#if defined(_MSC_VER)
#include <Windows.h>
#elif defined(__linux__) // Linux
#include <iconv.h>
#endif

namespace DevLib
{

	std::wstring UTF8ToUnicode(const std::string &str)
	{
		std::wstring ret;

#if defined(_MSC_VER)

		ret.resize(MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0));
		MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), ret.data(), static_cast<int32_t>(ret.length()));
#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("WCHAR_T", "UTF-8");
		if (cd != (iconv_t)-1)
		{

			// 입력과 출력 버퍼 설정
			size_t inSize = str.size();
			size_t outSize = inSize * sizeof(wchar_t);
			char *inBuf = const_cast<char *>(str.data());
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret = std::wstring(reinterpret_cast<wchar_t *>(outBuf.data()), (outPtr - outBuf.data()) / sizeof(wchar_t));
			}

			// 메모리 해제
			iconv_close(cd);
		}

#endif
		return ret;
	}

	std::string UTF8ToMultiByte(const std::string &str)
	{
		std::string ret;

#if defined(_MSC_VER)

		// UTF8 -> Unicode
		std::wstring temp;
		temp.resize(MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0));
		MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), temp.data(), static_cast<int32_t>(temp.length()));

		// Unicode --> MultiByte
		ret.resize(WideCharToMultiByte(CP_ACP, 0, temp.data(), static_cast<int32_t>(temp.length()), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(CP_ACP, 0, temp.data(), static_cast<int32_t>(temp.length()), ret.data(), static_cast<int32_t>(ret.length()), nullptr, nullptr);

#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("CP949", "UTF-8"); // 예: SHIFT_JIS를 MultiByte로 사용
		if (cd != (iconv_t)-1)
		{

			// 입력과 출력 버퍼 설정
			size_t inSize = str.size();
			size_t outSize = inSize * 2; // 멀티바이트 문자열은 UTF-8보다 더 클 수 있음
			char *inBuf = const_cast<char *>(str.data());
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			// 변환 실행
			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret.assign(outBuf.data(), outBuf.size() - outSize);
			}
			// 자원 해제
			iconv_close(cd);
		}
#endif

		return ret;
	}

	std::string UnicodeToMultiByte(const std::wstring& str)
	{
		std::string ret;

#if defined(_MSC_VER)

		ret.resize(WideCharToMultiByte(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), ret.data(), static_cast<int32_t>(ret.length()), nullptr, nullptr);

#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("CP949", "WCHAR_T"); // 예: SHIFT_JIS를 MultiByte로 사용
		if (cd != (iconv_t)-1)
		{
			// 입력과 출력 버퍼 설정
			size_t inSize = str.size() * sizeof(wchar_t);							   // 입력 크기
			size_t outSize = inSize * 2;											   // 멀티바이트 문자열 크기 예측
			char *inBuf = reinterpret_cast<char *>(const_cast<wchar_t *>(str.data())); // wchar_t* -> char*
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			// 변환 실행
			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret.assign(outBuf.data(), outBuf.size() - outSize);
			}

			// 자원 해제
			iconv_close(cd);
		}
#endif

		return ret;
	}

	std::string UnicodeToUTF8(const std::wstring& str)
	{
		std::string ret;

#if defined(_MSC_VER)

		ret.resize(WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.length()), ret.data(), static_cast<int32_t>(ret.length()), nullptr, nullptr);

#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("UTF-8", "WCHAR_T"); // 예: SHIFT_JIS를 MultiByte로 사용
		if (cd != (iconv_t)-1)
		{
			// 입력과 출력 버퍼 설정
			size_t inSize = str.size() * sizeof(wchar_t);							   // 입력 크기
			size_t outSize = inSize * 2;											   // 멀티바이트 문자열 크기 예측
			char *inBuf = reinterpret_cast<char *>(const_cast<wchar_t *>(str.data())); // wchar_t* -> char*
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			// 변환 실행
			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret.assign(outBuf.data(), outBuf.size() - outSize);
			}

			// 자원 해제
			iconv_close(cd);
		}
#endif

		return ret;
	}

	std::wstring MultiByteToUnicode(const std::string &str)
	{
		std::wstring ret;

#if defined(_MSC_VER)

		ret.resize(MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0));
		MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), ret.data(), static_cast<int32_t>(ret.length()));

#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("WCHAR_T", "CP949"); // CP949(MultiByte) -> WCHAR_T(유니코드)
		if (cd != (iconv_t)-1)
		{
			// 입력과 출력 버퍼 설정
			size_t inSize = str.size();
			size_t outSize = inSize * sizeof(wchar_t); // 예상 유니코드 크기
			char *inBuf = const_cast<char *>(str.data());
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			// 변환 작업
			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret.assign(reinterpret_cast<wchar_t *>(outBuf.data()), (outPtr - outBuf.data()) / sizeof(wchar_t));
			}
		}
#endif

		return ret;
	}

	std::string MultiByteToUTF8(const std::string& str)
	{
		std::string ret;

#if defined(_MSC_VER)

		// MultiByte --> Unicode
		std::wstring temp;
		temp.resize(MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), nullptr, 0));
		MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int32_t>(str.length()), temp.data(), static_cast<int32_t>(temp.length()));

		// Unicode --> UTF8
		ret.resize(WideCharToMultiByte(CP_UTF8, 0, temp.data(), static_cast<int32_t>(temp.length()), nullptr, 0, nullptr, nullptr));
		WideCharToMultiByte(CP_UTF8, 0, temp.data(), static_cast<int32_t>(temp.length()), ret.data(), static_cast<int32_t>(ret.length()), nullptr, nullptr);

#elif defined(__linux__) // Linux
		// iconv 초기화
		iconv_t cd = iconv_open("UTF-8", "CP949"); // 예: SHIFT_JIS를 MultiByte로 사용
		if (cd != (iconv_t)-1)
		{
			// 입력과 출력 버퍼 설정
			size_t inSize = str.size();
			size_t outSize = inSize * 2; // 멀티바이트 문자열은 UTF-8보다 더 클 수 있음
			char *inBuf = const_cast<char *>(str.data());
			std::vector<char> outBuf(outSize);
			char *outPtr = outBuf.data();

			// 변환 실행
			size_t converted = iconv(cd, &inBuf, &inSize, &outPtr, &outSize);
			if (converted != (size_t)-1)
			{
				ret.assign(outBuf.data(), outBuf.size() - outSize);
			}
			// 자원 해제
			iconv_close(cd);
		}
#endif

		return ret;
	}
	
}
