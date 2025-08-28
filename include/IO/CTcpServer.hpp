#pragma once

#include <optional>

#include "CSocketReceiver.hpp"
#include "../Base/CLocker.hpp"
#include "../Base/DevLibCallback.hpp"

namespace DevLib
{
	namespace IO
	{
		class CTcpServer
		{
		public:
			CTcpServer();
			virtual ~CTcpServer();

			bool CreateTcpServer(uint16_t port, const std::string& bindIP = std::string(), bool bBlockMode = true, int32_t nListen = 5);
			void CloseTcpServer();
			void SetBufferSize(size_t sizeBuffer = 8192);
			size_t GetBufferSize() const { return m_sizeBuffer; }

			int32_t Send(void_const_ptr pSrc, size_t size, int32_t nFlag = 0);
			int32_t SendTo(const std::string& ip, uint16_t port, void_const_ptr pSrc, size_t size, int32_t nFlag = 0);

			std::vector<CSocket> GetClientInfo();
			std::optional<CSocket> GetClientSocket(const std::string& ip, uint16_t port);
			void DisconnectClient(const std::string &ip, uint16_t port);

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)
			EnableReturnCallback(bool, Accept, CSocket& client)
			EnableCallback(ReceiveData, const CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)

			bool SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count) const;

			// TLS
			bool EnableTLS(const std::string& Certificate, const std::string& PrivateKey);
			bool EnableTLSFile(const std::string& CertificateFile = "./server.crt", const std::string& PrivateKeyFile = "./server.key");
			bool IsEnableTLS() const;

			bool EnableMTLS(const std::string& Certificate);
			bool EnableMTLSFile(const std::string& CertificateFile);

		private:
			size_t m_sizeBuffer;
			CThread m_threadListen;
			CSocket	m_socketListen;

			void WatchDogConnection();
			void OnDataClient(CSocket& client, void_ptr pData, int32_t size) const;

			void OnClientSocketState(CSocket& client, uint32_t state) const;

			CLocker<std::list<CSocketReceiver>> m_listClient;
		public:
			CTcpServer(const CTcpServer&) = delete;
			CTcpServer(CTcpServer&&) = delete;

			CTcpServer& operator=(const CTcpServer&) = delete;
			CTcpServer& operator=(CTcpServer&&) = delete;
		};
	}
}