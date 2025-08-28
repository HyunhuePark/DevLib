#include "../../include/IO/CSocket.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef _MSC_VER
#pragma comment(lib, "Crypt32.lib")
#endif

#include "../../include/IO/DevLibSocketUtility.hpp"

#include "../../include/Base/CLocker.hpp"

namespace DevLib
{
	namespace IO
	{

		std::once_flag _routine_socket_onceFlag;
		std::unique_ptr<CLocker<CSocket::listSocket>> _routine_manage_socket;

		std::atomic_int32_t _tls_used_ = 0;

		CSocket::CSocket()
			: m_hSocket(std::make_shared<socket_t>(-1))
			, m_ctxSSL(nullptr)
			, m_ssl(nullptr)
			, m_bmtls(false)
		{
			std::call_once(_routine_socket_onceFlag, []
				{
					_routine_manage_socket = std::make_unique<CLocker<listSocket>>();
				}
			);

#ifdef _MSC_VER
			SocketUtility::InitWinSock();
#endif

			RegisterCallbackReceive(&CSocket::OnReceive, this);

			memset(&m_sockAddrBind, 0, sizeof m_sockAddrBind);
			memset(&m_sockAddrRemote, 0, sizeof m_sockAddrRemote);
		}

		CSocket::~CSocket()
		{
			if (m_hSocket.use_count() == 1)
			{
				Destroy();
				TLSDestroy();
			}
		}

		bool CSocket::Create(const int32_t sockType)
		{
			bool bRet = false;

			if (!IsCreated())
			{
				m_sockType = sockType;

				int32_t protocol = 0;

				switch (GetSocketType())
				{
				case SOCK_TYPE_TCP:
					protocol = PROTOCOL_TCP;
					break;
				case SOCK_TYPE_UDP:
					protocol = PROTOCOL_UDP;
					break;
				case SOCK_TYPE_RAW:
					protocol = PROTOCOL_IP;
					break;
				case SOCK_TYPE_RAW_ICMP:
					m_sockType = SOCK_TYPE_RAW;
					protocol = PROTOCOL_ICMP;
					break;
				case SOCK_TYPE_RAW_UDP:
					m_sockType = SOCK_TYPE_RAW;
					protocol = PROTOCOL_UDP;
					break;
				default:
					break;
				}

				*m_hSocket = socket(AF_INET, m_sockType, protocol);

				if (IsCreated())
				{
					// Default Reuse true
					SetSockReuse(true);

					// Default Nagle Off
					SetSockNagle(false);

					// TCP Block
					if (sockType == SOCK_TYPE_TCP)
					{
						SetSockBlockIO(true);
					}
					else
					{
						SetSockBlockIO(false);
					}

					bRet = true;

					// Register Manage Socket
					_routine_manage_socket->Lock();
					_routine_manage_socket->get().push_back(this);
					_routine_manage_socket->UnLock();
				}
			}

			return bRet;
		}

		void CSocket::Destroy()
		{
			if (IsCreated())
			{
				StopWatchDog();
#ifdef _MSC_VER
				closesocket(*m_hSocket);
#elif defined(__linux__) // Linux
				close(*m_hSocket);
#endif
				* m_hSocket = -1;

				// Delete Manage Thread
				_routine_manage_socket->Lock();
				_routine_manage_socket->get().remove(this);
				_routine_manage_socket->UnLock();
			}
		}

		bool CSocket::Bind(const uint16_t port /*= 0*/, const std::string& ip /*= std::string()*/)
		{
			bool bRet = false;

			if (IsCreated())
			{
				memset(&m_sockAddrBind, 0, sizeof(SOCK_ADDR));
				m_sockAddrBind.family = AF_INET;
				m_sockAddrBind.port = htons(port);

				if (ip.empty())
				{
					m_sockAddrBind.addr = ADDR_ANY;
				}
				else
				{
					m_sockAddrBind.addr = SocketUtility::ipToAddr(ip);
				}

#ifdef _MSC_VER
				if (bind(*m_hSocket, reinterpret_cast<SOCKADDR*>(&m_sockAddrBind), sizeof m_sockAddrBind) != SOCKET_ERROR)
				{
					bRet = true;
				}
#elif defined(__linux__) // Linux
				if (bind(*m_hSocket, reinterpret_cast<sockaddr*>(&m_sockAddrBind), sizeof m_sockAddrBind) != SOCKET_ERROR)
				{
					bRet = true;
				}
#endif
			}

			return bRet;
		}

		bool CSocket::Listen(const int32_t nListen /*= 5*/) const
		{
			return listen(*m_hSocket, nListen) == 0 ? true : false;
		}

		bool CSocket::Connect(const uint16_t port, const std::string& ip)
		{
			bool bRet = false;

			if (IsCreated())
			{
				memset(&m_sockAddrRemote, 0, sizeof(SOCK_ADDR));
				m_sockAddrRemote.family = AF_INET;
				m_sockAddrRemote.addr = SocketUtility::ipToAddr(ip);
				m_sockAddrRemote.port = htons(port);

				if (connect(*m_hSocket, reinterpret_cast<sockaddr*>(&m_sockAddrRemote), sizeof(SOCK_ADDR)) != SOCKET_ERROR)
				{
#ifdef _MSC_VER
					int len = sizeof(sockaddr);
#elif defined(__linux__) // Linux
					socklen_t len = sizeof(sockaddr);
#endif
					getsockname(*m_hSocket, reinterpret_cast<sockaddr*>(&m_sockAddrBind), &len);

					bRet = true;
				}
			}

			return bRet;
		}

		bool CSocket::Accept(CSocket& comm) const
		{
			bool bRet = false;

			memset(&comm.m_sockAddrBind, 0, sizeof(SOCK_ADDR));
			memcpy(&comm.m_sockAddrBind, &m_sockAddrBind, sizeof(SOCK_ADDR));
			memset(&comm.m_sockAddrRemote, 0, sizeof(SOCK_ADDR));

#ifdef _MSC_VER
			int32_t addrSize = sizeof m_sockAddrBind;

			const SOCKET clientSocket = accept(*m_hSocket, reinterpret_cast<sockaddr*>(&comm.m_sockAddrRemote), &addrSize);
#elif defined(__linux__) // Linux
			socklen_t len = sizeof(m_sockAddrBind);
			const int clientSocket = accept(*m_hSocket, reinterpret_cast<sockaddr*>(&comm.m_sockAddrRemote), &len);
			constexpr int INVALID_SOCKET = -1;
#endif

			if (clientSocket != INVALID_SOCKET)
			{
				comm.m_sockType = m_sockType;
				*comm.m_hSocket = clientSocket;

				// Default Nagle Off
				comm.SetSockNagle(false);

				bRet = true;
			}

			return bRet;
		}

		int32_t CSocket::Recv(const void_const_ptr pDest, const size_t size, const int32_t nFlag) const
		{
			if (m_ssl)
			{
				return SSL_read(static_cast<SSL*>(m_ssl), static_cast<char*>(const_cast<void*>(pDest)), static_cast<int32_t>(size));
			}

			return recv(*m_hSocket, static_cast<char*>(const_cast<void*>(pDest)), static_cast<int32_t>(size), nFlag);
		}

		int32_t CSocket::RecvFrom(const void_const_ptr pDest, const size_t size, std::string& ip, uint16_t& port, const int32_t nFlag /*= 0*/)
		{
			SOCK_ADDR addr{};
			memset(&addr, 0, sizeof addr);
			const int32_t nRead = RecvFrom(pDest, size, addr, nFlag);

			if (nRead != SOCKET_ERROR)
			{
				ip = SocketUtility::addrToIP(addr.addr);
				port = htons(addr.port);
				m_sockAddrRemote.family = m_sockAddrBind.family;
				m_sockAddrRemote.port = addr.port;
				m_sockAddrRemote.addr = addr.addr;
			}

			return nRead;
		}

		int32_t CSocket::RecvFrom(const void_const_ptr pDest, const size_t size, SOCK_ADDR& addr, const int32_t nFlag /*= 0*/)
		{
			int32_t ret = 0;
#ifdef _MSC_VER
			int32_t len = sizeof addr;
			ret = recvfrom(*m_hSocket, static_cast<char*>(const_cast<void*>(pDest)), static_cast<int32_t>(size), nFlag, reinterpret_cast<SOCKADDR*>(&addr), &len);
#elif defined(__linux__) // Linux
			socklen_t len = sizeof addr;
			ret = recvfrom(*m_hSocket, static_cast<char*>(const_cast<void*>(pDest)), size, nFlag, reinterpret_cast<sockaddr*>(&addr), &len);
#endif

			m_sockAddrRemote.family = m_sockAddrBind.family;
			m_sockAddrRemote.port = addr.port;
			m_sockAddrRemote.addr = addr.addr;

			return ret;
		}

		int32_t CSocket::Send(const void_const_ptr pSrc, const size_t size, const int32_t nFlag) const
		{
			if (m_ssl)
			{
				return SSL_write(static_cast<SSL*>(m_ssl), pSrc, static_cast<int32_t>(size));
			}

			return send(*m_hSocket, static_cast<char*>(const_cast<void*>(pSrc)), static_cast<int32_t>(size), nFlag);
		}

		int32_t CSocket::SendTo(const void_const_ptr pSrc, const size_t size, const std::string& ip, const uint16_t port, const int32_t nFlag /*= 0*/) const
		{
			SOCK_ADDR addr{};
			memset(&addr, 0, sizeof addr);
			addr.addr = SocketUtility::ipToAddr(ip);
			addr.family = AF_INET;
			addr.port = htons(port);

			return SendTo(pSrc, size, addr, nFlag);
		}

		int32_t CSocket::SendTo(const void_const_ptr pSrc, const size_t size, SOCK_ADDR& addr, const int32_t nFlag /*= 0*/) const
		{
#ifdef _MSC_VER
			return sendto(*m_hSocket, static_cast<char*>(const_cast<void*>(pSrc)), static_cast<int32_t>(size), nFlag, reinterpret_cast<SOCKADDR*>(&addr), sizeof addr);
#elif defined(__linux__) // Linux
			return sendto(*m_hSocket, static_cast<char*>(const_cast<void*>(pSrc)), static_cast<int32_t>(size), nFlag, reinterpret_cast<sockaddr*>(&addr), sizeof addr);
#endif
		}

		std::string CSocket::GetAddressRemoteIP() const
		{
			return SocketUtility::addrToIP(GetAddressRemote());
		}

		uint16_t CSocket::GetAddressRemotePort() const
		{
			return SocketUtility::addrToPort(GetAddressRemote());
		}
		CSocket::InterfaceInfo CSocket::GetInterfaceAddress()
		{
			return SocketUtility::GetInterfaceAddress();
		}

		bool CSocket::StartWatchDog()
		{
			return m_sockThread.StartThread(&CSocket::WatchDogRecv, this);
		}

		void CSocket::StopWatchDog()
		{
			m_sockThread.StopThread();
		}

		bool CSocket::TLSCreateContext(bool bServer)
		{
			bool bRet = false;

			// Init OpenSSL
			++_tls_used_;

			if (_tls_used_ == 1)
			{
				SSL_library_init();
				OpenSSL_add_ssl_algorithms();
				SSL_load_error_strings();
			}

			if (!m_ctxSSL)
			{
				if (bServer)
				{
					m_ctxSSL = SSL_CTX_new(TLS_server_method());

				}
				else
				{
					m_ctxSSL = SSL_CTX_new(TLS_client_method());
				}

				if (m_ctxSSL)
				{
					bRet = true;

					if( bServer)
					{
						SSL_CTX_set_cipher_list(static_cast<SSL_CTX*>(m_ctxSSL), "HIGH:!aNULL:!MD5:!RC4");
					}
				}
			}

			return bRet;
		}

		bool CSocket::TLSCreateSSL()
		{
			bool bRet = false;

			// TLS
			if (m_ctxSSL)
			{
				m_ssl = SSL_new(static_cast<SSL_CTX*>(m_ctxSSL));
				if (SSL_set_fd(static_cast<SSL*>(m_ssl), static_cast<int32_t>(*m_hSocket)) == 1)
				{
					bRet = true;
				}
				else
				{
					SSL_free(static_cast<SSL*>(m_ssl));
				}
			}

			return bRet;
		}

		void CSocket::TLSShutdownSSL()
		{
			if( m_ssl )
			{
				SSL_shutdown(static_cast<SSL*>(m_ssl));
				SSL_free(static_cast<SSL*>(m_ssl));
				m_ssl = nullptr;
			}
		}

		bool CSocket::TLSCreateSSL(handle_t ctxSSL)
		{
			bool bRet = false;
			// TLS
			if (ctxSSL)
			{
				m_ssl = SSL_new(static_cast<SSL_CTX*>(ctxSSL));

				// UDP 
				if(m_sockType == SOCK_DGRAM )
				{
					const auto bio = BIO_new_dgram(static_cast<int32_t>(*m_hSocket), BIO_NOCLOSE);
					SSL_set_bio(static_cast<SSL*>(m_ssl), bio, bio);
				}
				else
				{
					if (SSL_set_fd(static_cast<SSL*>(m_ssl), static_cast<int32_t>(*m_hSocket)) == 1)
					{
						bRet = true;
					}
					else
					{
						SSL_free(static_cast<SSL*>(m_ssl));
					}
				}
			}

			return bRet;
		}

		bool CSocket::TLSLoadCertificateAndKey(const std::string& Certificate, const std::string& PrivateKey) const
		{
			bool bRet = false;

			if( m_ctxSSL)
			{
				// 인증서 로드 (메모리 기반)
				BIO* certBio = BIO_new_mem_buf(Certificate.c_str(), -1);
				BIO* keyBio = BIO_new_mem_buf(PrivateKey.c_str(), -1);

				X509* cert = PEM_read_bio_X509(certBio, nullptr, nullptr, nullptr);
				EVP_PKEY* key = PEM_read_bio_PrivateKey(keyBio, nullptr, nullptr, nullptr);

				if (cert)
				{
					if (SSL_CTX_use_certificate(static_cast<SSL_CTX*>(m_ctxSSL), cert) > 0)
					{
						if (key)
						{
							if (SSL_CTX_use_PrivateKey(static_cast<SSL_CTX*>(m_ctxSSL), key) > 0)
							{
								bRet = true;
							}
						}
					}
				}

				// 리소스 정리
				X509_free(cert);
				BIO_free(certBio);
				BIO_free(keyBio);
				EVP_PKEY_free(key);
			}

			return bRet;
		}

		bool CSocket::TLSLoadCertificateAndKeyFile(const std::string& CertificateFile, const std::string& PrivateKeyFile) const
		{
			bool bRet = false;

			if (m_ctxSSL)
			{
				if (SSL_CTX_use_certificate_file(static_cast<SSL_CTX*>(m_ctxSSL), CertificateFile.c_str(), SSL_FILETYPE_PEM) > 0)
				{
					if (SSL_CTX_use_PrivateKey_file(static_cast<SSL_CTX*>(m_ctxSSL), PrivateKeyFile.c_str(), SSL_FILETYPE_PEM) > 0)
					{
						bRet = true;
					}
				}
			}

			return bRet;
		}

		bool CSocket::TLSEnableMutualAuthentication(const std::vector<std::string>& caCertificates, int32_t depth )
		{
			bool bRet = false;

			if (m_ctxSSL)
			{
				for (const auto& certString : caCertificates) 
				{
					BIO* bio = BIO_new_mem_buf(certString.c_str(), -1);
					X509* caCert = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
					BIO_free(bio);

					if (caCert && SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(m_ctxSSL)))
					{
						X509_STORE* store = SSL_CTX_get_cert_store(static_cast<SSL_CTX*>(m_ctxSSL));
						if (X509_STORE_add_cert(store, caCert)>0) 
						{
							bRet = true;
							m_bmtls = true;
						}

						X509_free(caCert); // 인증서 해제
					}
				}

				// Require client certificate
				SSL_CTX_set_verify(static_cast<SSL_CTX*>(m_ctxSSL), SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
				SSL_CTX_set_verify_depth(static_cast<SSL_CTX*>(m_ctxSSL), depth);

			}

			return bRet;
		}

		bool CSocket::TLSEnableMutualAuthenticationFile(const std::string& caCertificate, int32_t depth)
		{
			bool bRet = false;

			if (m_ctxSSL)
			{
				if (SSL_CTX_load_verify_locations(static_cast<SSL_CTX*>(m_ctxSSL), caCertificate.c_str(), nullptr) > 0)
				{
					bRet = true;
					m_bmtls = true;

					// Require client certificate
					SSL_CTX_set_verify(static_cast<SSL_CTX*>(m_ctxSSL), SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
					SSL_CTX_set_verify_depth(static_cast<SSL_CTX*>(m_ctxSSL), depth);
				}
			}

			return bRet;
		}


		bool CSocket::TLSConnect() const
		{
			bool bRet = false;

			if (SSL_connect(static_cast<SSL*>(m_ssl)) > 0)
			{
				bRet = true;
			}

			return bRet;
		}

		bool CSocket::TLSAccept() const
		{
			bool bRet = false;
			if (SSL_accept(static_cast<SSL*>(m_ssl)) > 0 )
			{
				if (m_bmtls)
				{
					X509* clientCert = SSL_get_peer_certificate(static_cast<SSL*>(m_ssl));
					if (clientCert)
					{
						if (SSL_get_verify_result(static_cast<SSL*>(m_ssl)) == X509_V_OK)
						{
							bRet = true;
						}

						X509_free(clientCert);
					}
				}
				else
				{
					bRet = true;
				}
			}
			else
			{
				ERR_print_errors_fp(stderr);
			}

			return bRet;
		}

		bool CSocket::TLSBind() const
		{
			bool bRet = false;
			if (SSL_set_fd(static_cast<SSL*>(m_ssl), static_cast<int32_t>(*m_hSocket)) > 0)
			{
				bRet = true;
			}

			return bRet;
		}

		bool CSocket::TLSIsCreatedContext() const
		{
			return m_ctxSSL ? true : false;
		}

		bool CSocket::TLSIsCreatedSSL() const
		{
			return m_ssl ? true : false;
		}

		handle_t CSocket::TLSGetContext() const
		{
			return m_ctxSSL;
		}

		handle_t CSocket::TLSGetSSL() const
		{
			return m_ssl;
		}

		void CSocket::TLSDestroy()
		{
			if (m_ssl)
			{
				SSL_shutdown(static_cast<SSL*>(m_ssl));
				SSL_free(static_cast<SSL*>(m_ssl));
				m_ssl = nullptr;
			}

			if (m_ctxSSL)
			{
				SSL_CTX_free(static_cast<SSL_CTX*>(m_ctxSSL));
				m_ctxSSL = nullptr;

				--_tls_used_;
				if (_tls_used_ == 0)
				{
					EVP_cleanup();
				}
			}
		}

		void CSocket::SetSockNagle(const bool bUse) const
		{
			SocketUtility::SetSockNagle(GetSocketHandle(), bUse);
		}

		bool CSocket::SetSockBlockIO(const bool bBlock)
		{
			const bool currentBlockMode = m_bBlockMode;
			m_bBlockMode = bBlock;
			SocketUtility::SetSockBlockIO(GetSocketHandle(), bBlock);

			return currentBlockMode;
		}

		void CSocket::SetSockReuse(const bool bReuse) const
		{
			SocketUtility::SetSockReuse(GetSocketHandle(), bReuse);
		}

		bool CSocket::SetSockBroadcast(const bool bUse) const
		{
			bool bRet = false;
			if (m_sockType == SOCK_DGRAM)
			{
				bRet = SocketUtility::SetSockBroadcast(GetSocketHandle(), bUse);
			}
			return bRet;
		}

		bool CSocket::SetSockRecvAll(const bool bRecvAll) const
		{
			return SocketUtility::SetSockRecvAll(GetSocketHandle(), bRecvAll);
		}

		bool CSocket::SetSockKeepAlive(bool bUse, uint32_t idleSec, uint32_t intervalSec, uint32_t intervalCount) const
		{
			return SocketUtility::SetSockKeepAlive(GetSocketHandle(), bUse, idleSec, intervalSec, intervalCount);
		}

		bool CSocket::SetTimeoutRecv(uint32_t ms) const
		{
			return SocketUtility::SetTimeoutRecv(GetSocketHandle(), ms);
		}

		bool CSocket::SetTimeoutSend(uint32_t ms) const
		{
			return SocketUtility::SetTimeoutSend(GetSocketHandle(), ms);
		}

		uint32_t CSocket::GetTimeoutRecv() const
		{
			return SocketUtility::GetTimeoutRecv(GetSocketHandle());
		}

		uint32_t CSocket::GetTimeoutSend() const
		{
			return SocketUtility::GetTimeoutSend(GetSocketHandle());
		}

		bool CSocket::IsCreated() const
		{
			return *m_hSocket > 0 ? true : false;
		}

		bool CSocket::IsDestroyed() const
		{
			bool bRet = false;
			if (m_hSocket.use_count() == 1 && !IsCreated())
			{
				bRet = true;
			}

			return bRet;
		}

		bool CSocket::WaitForRecv(const uint32_t ms /*= 0xFFFFFFFF*/) const
		{
			bool bRet = false;
			bool bWork = true;

			// wait on Watchdog thread
			if (m_sockThread.IsRunThread())
			{
				const CThread* pCurrentThread = CThread::GetCurrentThread();

				if (pCurrentThread)
				{
					if (pCurrentThread->GetID() != m_sockThread.GetID())
					{
						bWork = false;
					}
				}
				else
				{
					bWork = false;
				}
			}

			if (IsCreated() & bWork)
			{
				const auto socket = *m_hSocket;
				fd_set read{};
				memset(&read, 0, sizeof(fd_set));

				FD_ZERO(&read);
				FD_SET(socket, &read);

				timeval time{};
				memset(&time, 0, sizeof(timeval));
				time.tv_sec = static_cast<long>(ms * 0.001);
				time.tv_usec = static_cast<long>(ms % 1000 * 1000);

				const int32_t ret = select(static_cast<int32_t>(socket) + 1, &read, nullptr, nullptr, &time);

				if (ret > 0)
				{
					if (FD_ISSET(socket, &read))
					{
						bRet = true;
					}
				}
			}

			return bRet;
		}

		bool CSocket::WaitForWritable(uint32_t ms) const
		{
			bool bRet = false;

			if (IsCreated())
			{
				fd_set write{};
				memset(&write, 0, sizeof(fd_set));

				FD_ZERO(&write);
				FD_SET(*m_hSocket, &write);

				timeval time{};
				memset(&time, 0, sizeof(timeval));
				time.tv_sec = static_cast<long>(ms * 0.001);
				time.tv_usec = static_cast<long>(ms % 1000 * 1000);

				const int32_t ret = select(static_cast<int32_t>(*m_hSocket) + 1, nullptr, &write, nullptr, &time);

				if (ret > 0)
				{
					if (FD_ISSET(*m_hSocket, &write))
					{
						bRet = true;
					}
				}
			}

			return bRet;
		}

		bool CSocket::MulticastTTL(const int32_t nTTL /*= 1*/) const
		{
			return SocketUtility::MulticastTTL(GetSocketHandle(), nTTL);
		}

		bool CSocket::MulticastJoin(const std::string& groupIP, const std::string& bindIP /*= std::string()*/) const
		{
			bool bRet = false;
			if (m_sockType == SOCK_DGRAM)
			{
				bRet = SocketUtility::MulticastJoin(GetSocketHandle(), groupIP, bindIP);
			}
			return bRet;
		}

		bool CSocket::MulticastLeave(const std::string& groupIP, const std::string& bindIP /*= std::string()*/) const
		{
			bool bRet = false;
			if (m_sockType == SOCK_DGRAM)
			{
				bRet = SocketUtility::MulticastLeave(GetSocketHandle(), groupIP, bindIP);
			}

			return bRet;
		}

		bool CSocket::MulticastLoopback(const bool bLoopback /*= true*/) const
		{
			return SocketUtility::MulticastLoopbackEnable(GetSocketHandle(), bLoopback);
		}

		bool CSocket::MulticastBind(const std::string& bindIP) const
		{
			return SocketUtility::MulticastBind(GetSocketHandle(), bindIP);
		}

		void CSocket::WatchDogRecv()
		{
			auto lockWatchDog = m_hSocket;

			if (IsCreated() == true)
			{
				while (m_sockThread.IsRunThread())
				{
					if (WaitForRecv())
					{
						if (m_callbackReceive) m_callbackReceive(*this);
					}
				}
			}
		}

		CSocket::CSocket(const CSocket& other) : m_bBlockMode(other.m_bBlockMode), m_sockType(other.m_sockType)
			, m_hSocket(other.m_hSocket), m_sockAddrRemote(other.m_sockAddrRemote), m_sockAddrBind(other.m_sockAddrBind)
			, m_ctxSSL(other.m_ctxSSL), m_ssl(other.m_ssl), m_bmtls(other.m_bmtls)
		{

		}

		CSocket& CSocket::operator=(const CSocket& other)
		{
			m_bBlockMode = other.m_bBlockMode;
			m_sockType = other.m_sockType;
			m_hSocket = other.m_hSocket;
			m_sockAddrRemote = other.m_sockAddrRemote;
			m_sockAddrBind = other.m_sockAddrBind;
			m_ctxSSL = other.m_ctxSSL;
			m_ssl = other.m_ssl;
			m_bmtls = other.m_bmtls;

			return *this;
		}
	}
}
