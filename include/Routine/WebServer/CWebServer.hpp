#pragma once

#include <map>
#include "../../IO/CTcpServer.hpp"
#include "../../Base/CTime.hpp"
#include "../../Base/CTimer.hpp"
#include "DevLibHTTPUtility.hpp"

namespace DevLib {
	namespace Routine
	{
		namespace HTTP
		{
			class CWebServer
			{
			public:
				CWebServer();
				virtual ~CWebServer() = default;

				bool HTTPOpen(uint16_t port, const std::string& bindIP = {}, uint32_t listenCount = 128);
				bool HTTPSOpen(uint16_t port, const std::string& CertificateFile = "./server.crt", const std::string& PrivateKeyFile = "./server.key", const std::string& bindIP = {}, uint32_t listenCount = 128);

				EnableCallback(Connection, const std::string& ip, uint16_t port)
				EnableCallback(DisConnection, const std::string& ip, uint16_t port)

				EnableCallback(RequestGet, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url)
				EnableCallback(RequestPost, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url, const std::string& payload)
				EnableCallback(RequestPut, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url, const std::string& payload)
				EnableCallback(RequestDelete, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url)
				EnableCallback(RequestHead, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url)
				EnableCallback(RequestOptions, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url)
				EnableCallback(RequestTrace, const DevLib::IO::CSocket& client, HeaderProperty& property, const std::string& url)
				EnableCallback(RequestEx, const DevLib::IO::CSocket& client, const std::string& request)

				IO::CTcpServer& GetTCPServer()
				{
					return m_server;
				}

			private:
				IO::CTcpServer m_server;

				DevLib::CTimer m_timerKeepAlive;
				CCriticalSectionEx m_lockClientStream;
				std::map<std::string, std::map<uint16_t, double>> m_keepAlive;
				void OnTimerCheckKeepAlive();

				void OnReceiveData(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size);

				void OnHTTPConnection(const std::string& ip, uint16_t port);
				void OnHTTPDisConnection(const std::string& ip, uint16_t port);
			};


		}


	}
}
