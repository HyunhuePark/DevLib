#pragma once

#include <string>
#include <map>
#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		namespace HTTP
		{
			using HeaderProperty = std::map<std::string, std::string>;

			std::string MakeHTTP(uint32_t code, const std::string& content = std::string(), const std::string& contentType = "text/html");
			std::string MakeHTTPEx(uint32_t code, const HeaderProperty& property, const std::string& content = std::string(),  const std::string& contentType = "text/html");
			std::string ReadResource(const std::string& path);

			std::string EncodeURI(const std::string& str);
			std::string DecodeURI(const std::string& str);

			std::string GetHTML404();

			std::string Base64Encode(const ::std::string &bindata);
			std::string Base64Decode(const ::std::string &ascdata);

			// Token
			std::string GeneratorToken(const std::string& issuer, const std::string& userName, uint64_t secondValid);
			std::string GeneratorToken(const std::string& publicKey, const std::string& privateKey, const std::string& issuer, const std::string& userName, uint64_t secondValid);

			bool VerifyToken(const std::string& token, const std::string& issuer);
			bool VerifyToken(const std::string& token, const std::string& publicKey, const std::string& issuer);
			
		}
	}
}