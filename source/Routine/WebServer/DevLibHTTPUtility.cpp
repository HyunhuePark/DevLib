
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4244)
#include <jwt-cpp/jwt.h>
#pragma warning(pop)
#elif defined(__linux__)// linux
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <jwt-cpp/jwt.h>
#pragma GCC diagnostic pop
#endif

#include "../../../include/Routine/WebServer/DevLibHTTPUtility.hpp"

#include "../../../include/IO/CFile.hpp"
#include "../../../include/3rd/base64/base64.hpp"
#include "../../../include/IO/DevLibTLSUtility.hpp"

#include <regex>
#include <sstream>



namespace DevLib {

	namespace Routine
	{
		namespace HTTP
		{
			std::string StatusCodeToString(uint32_t code)
			{
				std::string status;

				switch (code)
				{
				case 200:
					status = "200 OK";
					break;

				case 201:
					status = "201 Created";
					break;

				case 400:
					status = "400 Bad Request";
					break;

				case 401:
					status = "401 Unauthorized";
					break;

				case 403:
					status = "403 Forbidden";
					break;

				case 404:
				default:
					status = "404 Not Found";
					break;
				}

				return status;
			}

			std::string MakeHTTP(uint32_t code, const std::string& content, const std::string& contentType)
			{
				const std::string http = "HTTP/1.1 " + StatusCodeToString(code) + "\r\n" +
					"Connection: keep-alive\r\n" +
					"Keep-Alive: max=100,timeout=6000\r\n" +
					"Content-Length: " + std::to_string(content.length()) + "\r\n" +
					"Content-Type: " + contentType + "\r\n\r\n";

				return http + content;
			}

			std::string MakeHTTPEx(uint32_t code, const HeaderProperty& property, const std::string& content, const std::string& contentType)
			{
				std::string http = "HTTP/1.1 " + StatusCodeToString(code) + "\r\n";

				for (const auto& attribute : property)
				{
					http += attribute.first + ": " + attribute.second + "\r\n";
				}

				http += "Content-Length: " + std::to_string(content.length()) + "\r\n" +
					"Content-Type: " + contentType + "\r\n\r\n";

				return http + content;
			}

			std::string ReadResource(const std::string& path)
			{
				std::string result;
				IO::CFile file;

				const std::string toLocalPath = "./" + path;
				if (file.Open(toLocalPath, "rb"))
				{
					result.resize(file.GetFileSize());
					file.Read(result.data(), file.GetFileSize());
				}

				return result;
			}

			std::string EncodeURI(const std::string& str)
			{
				std::ostringstream oss;
				const std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

				for (auto& c : str)
				{
					if (std::regex_match(std::string{ c }, r))
					{
						oss << c;
					}
					else
					{
						oss << "%" << std::uppercase << std::hex << (0xff & c);
					}
				}
				return oss.str();
			}

			std::string DecodeURI(const std::string& str)
			{
				std::string decoded = str;
				std::smatch sm;

				auto dynamicLength = static_cast<int32_t>(decoded.size() - 2);

				if (decoded.size() < 3)
					return decoded;

				for (auto i = 0; i < dynamicLength; i++)
				{

					std::string haystack = decoded.substr(i, 3);

					if (std::regex_match(haystack, sm, std::regex("%[0-9A-F]{2}")))
					{
						haystack = haystack.replace(0, 1, "0x");
						std::string rc = { static_cast<char>(std::stoi(haystack, nullptr, 16)) };
						decoded = decoded.replace(decoded.begin() + i, decoded.begin() + i + 3, rc);
					}

					dynamicLength = static_cast<int32_t>(decoded.size() - 2);;
				}

				return decoded;
			}

			std::string GetHTML404()
			{
				std::string html = R"(<!doctype html>
			<html><head><title> 404 Not Found. </title></head>
			<body>
			<h1>404 Not Found.</h1><p>"
					"</body> </html>")";

				return html;
			}

			std::string Base64Encode(const ::std::string& binData)
			{
				return base64::to_base64(binData);
			}

			std::string Base64Decode(const ::std::string& ascData)
			{
				return base64::from_base64(ascData);
			}


			std::string GeneratorToken(const std::string& issuer, const std::string& userName, uint64_t secondValid)
			{
				return GeneratorToken(DevLib::IO::TLS::GetRootCA(), DevLib::IO::TLS::GetRootKey(), issuer, userName, secondValid);
			}

			std::string GeneratorToken(const std::string& publicKey, const std::string& privateKey, const std::string& issuer, const std::string& userName, uint64_t secondValid)
			{
				std::string token;
				try
				{
					// JWT 생성
					token = jwt::create()
						.set_issuer(issuer) // 발급자
						.set_type("JWT")                // 토큰 타입
						.set_payload_claim(userName, jwt::claim(std::string("12345"))) // 사용자 정보
						.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds(secondValid)) // 만료 시간
						.sign(jwt::algorithm::rs256(publicKey, privateKey, "", "")); // RSA-SHA256 알고리즘으로 서명
				}
				catch (const jwt::error::rsa_exception& e)
				{
					// RSA 관련 예외 처리
					UNUSED(e);
				}
				catch (const std::exception& e)
				{
					// 기타 예외 처리
					UNUSED(e);
				}

				return token;
			}

			bool VerifyToken(const std::string& token, const std::string& issuer)
			{
				return VerifyToken(token, DevLib::IO::TLS::GetRootCA(), issuer);
			}

			bool VerifyToken(const std::string& token, const std::string& publicKey, const std::string& issuer)
			{
				bool bRet = false;

				// 공개 키를 사용한 검증
				try
				{
					const auto decoded = jwt::decode(token);
					const auto verifier = jwt::verify()
						.allow_algorithm(jwt::algorithm::rs256(publicKey, "", ""))
						.with_issuer(issuer);

					verifier.verify(decoded);
					bRet = true;

				}
				catch (const jwt::error::token_verification_exception& e) 
				{
					// JWT 검증 관련 예외 처리
					UNUSED(e);
				}

				return bRet;
			}
		}
	}
}