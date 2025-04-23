#pragma once

#include "DevLibSocketProtocol.hpp"
#include "../Base/DevLibCallback.hpp"
#include "../Base/CThread.hpp"
#include <map>

namespace DevLib
{
	namespace IO
	{
		class CSocket
		{
		public:
			CSocket();
			virtual ~CSocket();

			bool Create(int32_t sockType = SOCK_TYPE_TCP);

			bool Bind(uint16_t port = 0, const std::string& ip = std::string());
			bool Listen(int32_t nListen = 5) const;
			bool Connect(uint16_t port, const std::string& ip);
			bool Accept(CSocket& comm) const;

			bool WaitForRecv(uint32_t ms = 0xFFFFFFFF) const;
			bool WaitForWritable(uint32_t ms = 0x00000000) const;

			int32_t Recv(void_const_ptr pDest, size_t size, int32_t nFlag = 0) const;
			int32_t RecvFrom(void_const_ptr pDest, size_t size, std::string& ip, uint16_t& port, int32_t nFlag = 0);
			int32_t RecvFrom(void_const_ptr pDest, size_t size, SOCK_ADDR& addr, int32_t nFlag = 0);
			int32_t Send(void_const_ptr pSrc, size_t size, int32_t nFlag = 0) const;
			int32_t SendTo(void_const_ptr pSrc, size_t size, const std::string& ip, uint16_t port, int32_t nFlag = 0) const;
			int32_t SendTo(void_const_ptr pSrc, size_t size, SOCK_ADDR& addr, int32_t nFlag = 0) const;

			void Destroy();
			bool IsCreated() const;
			bool IsDestroyed() const;

			int32_t GetSocketType() const { return m_sockType; }
			socket_t GetSocketHandle() const { return *m_hSocket; }
			SOCK_ADDR GetAddressBind() const { return m_sockAddrBind; }
			SOCK_ADDR GetAddressRemote() const { return m_sockAddrRemote; }
			std::string GetAddressRemoteIP() const;
			uint16_t GetAddressRemotePort() const;

			using InterfaceInfo = std::map<std::string, std::vector<std::string>>;
			static InterfaceInfo GetInterfaceAddress();

			using listSocket = std::list<CSocket*>;

			bool StartWatchDog();
			void StopWatchDog();

			// TLS
			bool TLSCreateContext(bool bServer);
			bool TLSCreateSSL();
			void TLSShutdownSSL();
			bool TLSCreateSSL(handle_t ctxSSL);
			bool TLSLoadCertificateAndKey(const std::string& Certificate, const std::string& PrivateKey) const;
			bool TLSLoadCertificateAndKeyFile(const std::string& CertificateFile = "./server.crt", const std::string& PrivateKeyFile = "./server.key") const;

			bool TLSEnableMutualAuthentication(const std::vector<std::string>& caCertificate, int32_t depth = 1);
			bool TLSEnableMutualAuthenticationFile(const std::string& caCertificate, int32_t depth = 1);

			bool TLSConnect() const;
			bool TLSAccept() const;
			bool TLSBind() const;

			bool TLSIsCreatedContext() const;
			bool TLSIsCreatedSSL() const;

			handle_t TLSGetContext();
			handle_t TLSGetSSL();

			void TLSDestroy();

			EnableCallback(Receive, CSocket& sock)

		public:
			bool MulticastTTL(int32_t nTTL = 1) const;
			bool MulticastJoin(const std::string& groupIP, const std::string& bindIP = std::string()) const;
			bool MulticastLeave(const std::string& groupIP, const std::string& bindIP = std::string()) const;
			bool MulticastLoopback(bool bLoopback = true) const;
			bool MulticastBind(const std::string& bindIP) const;

			void SetSockNagle(bool bUse) const;
			bool SetSockBlockIO(bool bBlock);
			void SetSockReuse(bool bReuse) const;
			bool SetSockBroadcast(bool bUse) const;
			bool SetSockRecvAll(bool bRecvAll = true) const;
			bool SetSockKeepAlive(bool bUse, uint32_t idleSec = 60, uint32_t intervalSec = 1, uint32_t intervalCount = 10) const;

			bool SetTimeoutRecv(uint32_t ms);
			bool SetTimeoutSend(uint32_t ms) const;
			uint32_t GetTimeoutRecv();
			uint32_t GetTimeoutSend() const;

		 private:
			bool m_bBlockMode{}; // CurrentBlockMode
			int32_t m_sockType{}; // Socket Type
			sharedSocket m_hSocket; // Socket
			SOCK_ADDR m_sockAddrRemote{}; // SocketAddress
			SOCK_ADDR m_sockAddrBind{}; // SocketAddress

			// TLS
			handle_t m_ctxSSL;
			handle_t m_ssl;
			bool m_bmtls;

			CThread m_sockThread;
			void WatchDogRecv(); // watchdog for recv

		public:
			CSocket(const CSocket& other);
			CSocket(CSocket&&) = default;

			CSocket& operator=(const CSocket& other);
			CSocket& operator=(CSocket&&) = default;
		};
	}
}
