#pragma once

#include "CSocketReceiver.hpp"
#include "../Base/CCriticalSection.hpp"
#include "../Base/CTimer.hpp"
#include "../Base/DevLibTypes.hpp"

namespace DevLib
{
	namespace IO
	{
		class CTcpClient
		{
		public:
			CTcpClient();
			virtual ~CTcpClient();

			bool CreateTcpClient(std::string serverIP, uint16_t serverPort, uint16_t bindPort = 0, const std::string& bindIP = std::string(), bool bBlockMode = true, uint32_t timeAutoConnection = 1000);
			void Destroy();
			void Disconnect();
			bool IsCreated() const { return m_bCreatedClient; }

			int32_t Recv(void_const_ptr pDest, size_t size, int32_t nFlag = 0);
			int32_t Send(void_const_ptr pSrc, size_t size, int32_t nFlag = 0);

			void SetBufferSize(size_t sizeBuffer = 8192);

			size_t GetBufferSize() const { return m_sizeBuffer; }

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)
			EnableCallback(ReceiveData, const CSocket& sock, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)

			bool SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count);

			// TLS
			bool EnableTLS();
			bool EnableTLS(const std::string& Certificate, const std::string& PrivateKey, const std::string& CaCertificate = std::string());
			bool EnableTLSFile(const std::string& CertificateFile , const std::string& PrivateKeyFile , const std::string& CaCertificateFile = std::string());
			bool IsEnableTLS();


		private:
			size_t m_sizeBuffer;
			CSocketReceiver	m_socket;
			std::string m_serverIP = std::string();
			uint16_t m_serverPort;

			DevLib::CCriticalSection m_csConnection;
			uint32_t m_timeAutoConnection;

			bool m_bCreatedClient;
			CTimer m_timerConnection;
			void OnTimerAutoConnection();

			void OnDataServer(CSocket& sock, void_ptr pData, int32_t size) const;
			void OnServerSate(CSocket& server, uint32_t state);

		public:
			CTcpClient(const CTcpClient&) = delete;
			CTcpClient(CTcpClient&&) = delete;

			CTcpClient& operator=(const CTcpClient&) = delete;
			CTcpClient& operator=(CTcpClient&&) = delete;

			CSocket* operator->()
			{
				return &m_socket.GetSocket();
			}

			CSocket& operator*()
			{
				return m_socket.GetSocket();
			}
		};
	}
}