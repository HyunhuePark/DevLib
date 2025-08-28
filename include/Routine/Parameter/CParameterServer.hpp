#pragma once

#include <map>
#include <string>

#include "DevLibParameterType.hpp"
#include "../WebServer/CWebServer.hpp"
#include "../../IO/CTcpServer.hpp"

namespace DevLib {
	namespace Routine
	{
		class CParameterServer
		{
		public:
			CParameterServer();
			virtual ~CParameterServer() = default;

			bool ServerOpen(uint16_t port);

			template <typename Type>
			void RegisterParameter(const std::string& group, const std::string& parameterName, Type&& value);
			void SaveParameter() const;

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)

		private:
			HTTP::CWebServer m_server;

			std::map<std::string, std::map<std::string, CParamType::SharedPtr>> m_params;

			void OnRequestGET(const IO::CSocket& client, HTTP::HeaderProperty& property, const std::string& url);
			void OnRequestUser(const IO::CSocket& client, const std::string& url);

			void OnChangeParameter(const std::string& group, const std::string& parameter, std::string value);

			std::string MakeJSon() const;
			std::string MakeHTTP(uint32_t code, const std::string& html, const std::string& contentType = "text/html") const;
			std::string Make404() const;
			std::string MakeParameterContents() const;
			std::string MakeMain() const;
			std::string MakeRedirect(const std::string& hostAddr) const;

			//void ParserCommand(IO::CSocket& client, std::string strCommand);
			//std::string m_bufferCommand;
			//void OnReceiveData(IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size);
		};

		template <typename Type>
		void CParameterServer::RegisterParameter(const std::string& group, const std::string& parameterName, Type&& value)
		{
			m_params[group][parameterName] = std::make_shared<CParamValue<Type>>(value);
		}
	}
}