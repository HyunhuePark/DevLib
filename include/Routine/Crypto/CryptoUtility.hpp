#pragma once

#include <string>
#include <vector>
#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		namespace Crypto
		{
			std::vector<uint8_t> GeneratorKey();
			std::vector<uint8_t> GeneratorInitialVector();

			bool EncryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const uint8_t* pSrc, size_t srcSize, std::vector<uint8_t>& dest);
			bool EncryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& src, std::vector<uint8_t>& dest);
			bool DecryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const uint8_t* pSrc, size_t srcSize, std::vector<uint8_t>& dest);
			bool DecryptionAES(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv, const std::vector<uint8_t>& src, std::vector<uint8_t>& dest);

			std::vector<uint8_t> SHA256(const std::vector<uint8_t>& in, bool bUppercase = false);
			std::vector<uint8_t> SHA256(const std::string& in, bool bUppercase = false);

			std::vector<uint8_t> SHA512(const std::vector<uint8_t>& in, bool bUppercase = false);
			std::vector<uint8_t> SHA512(const std::string& in, bool bUppercase = false);

			std::vector<uint8_t> MD5(const std::vector<uint8_t>& in, bool bUppercase = false);
			std::vector<uint8_t> MD5(const std::string& in, bool bUppercase = false);

			std::string HexDataToString(const std::vector<uint8_t>& hexData, bool bUppercase = false);
			std::vector<uint8_t> HexStringToData(const std::string& hexString, bool bUppercase = false);

		}
	}
}
