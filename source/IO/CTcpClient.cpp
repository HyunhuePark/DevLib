#include <utility>

#include "../../include/IO/CTcpClient.hpp"
#include "../../include/IO/DevLibSocketUtility.hpp"
#include "../../include/IO/DevLibTLSUtility.hpp"

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
#include <errno.h>
#endif

namespace DevLib
{
	namespace IO
	{

		CTcpClient::CTcpClient()
			: m_sizeBuffer(8192)
			, m_serverPort(0)
			, m_timeAutoConnection(1000)
			, m_bCreatedClient(false)
		{
			m_socket.RegisterCallbackReceivedData(&CTcpClient::OnDataServer, this);

			RegisterCallbackConnection(&CTcpClient::OnConnection, this);
			RegisterCallbackDisConnection(&CTcpClient::OnDisConnection, this);
			RegisterCallbackReceiveData(&CTcpClient::OnReceiveData, this);
		}

		CTcpClient::~CTcpClient()
			= default;

		bool CTcpClient::CreateTcpClient(std::string serverIP, const uint16_t serverPort, const uint16_t bindPort /*= 0*/, const std::string& bindIP /*= std::string()*/, bool bBlockMode /*= true*/, const uint32_t timeAutoConnection /*= 1000*/)
		{
			bool bRet = m_socket.Create(SOCK_TYPE_TCP, GetBufferSize());

			if (bRet)
			{
				if (!bindIP.empty() && bindPort != 0) bRet = m_socket->Bind(bindPort, bindIP);


				if (bRet)
				{
					m_serverIP = std::move(serverIP);
					m_serverPort = serverPort;
					m_timeAutoConnection = timeAutoConnection;

					m_bCreatedClient = true;

					// Start Timer
					m_timerConnection.StartTimer(m_timeAutoConnection, m_timeAutoConnection, &CTcpClient::OnTimerAutoConnection, this);
				}
				else
				{
					m_socket->Destroy();
				}
			}

			return bRet;
		}

		void CTcpClient::Destroy()
		{
			if (m_timerConnection.IsRunTimer()) m_timerConnection.StopTimer();
			m_socket->StopWatchDog();
			m_socket->Destroy();
			m_bCreatedClient = false;
		}

		void CTcpClient::Disconnect()
		{
			m_socket->Destroy();
		}

		int32_t CTcpClient::Recv(const void_const_ptr pDest, const size_t size, const int32_t nFlag)
		{
			return m_socket.GetSocket().Recv(pDest, static_cast<int32_t>(size), nFlag);
		}

		int32_t CTcpClient::Send(const void_const_ptr pSrc, const size_t size, const int32_t nFlag)
		{
			int32_t ret = 0;
			if (m_socket->IsCreated())
			{
				ret = m_socket->Send(pSrc, static_cast<int32_t>(size), nFlag);
			}
			return ret;
		}

		void CTcpClient::SetBufferSize(const size_t sizeBuffer /*= 8192*/)
		{
			m_sizeBuffer = sizeBuffer;

			m_socket.SetBufferSize(sizeBuffer);
		}

		bool CTcpClient::SetKeepAliveTime(const uint32_t second, const uint32_t intervalSecond, const uint32_t count)
		{
			bool bRet = false;
			if (m_socket.IsCreated())
			{
				bRet = DevLib::IO::SocketUtility::SetSockKeepAlive(m_socket->GetSocketHandle(), true, second, intervalSecond, count);
			}
			return bRet;
		}

		bool CTcpClient::EnableTLS()
		{
			bool bRet = false;

			if( m_socket->TLSCreateContext(false) )
			{
				if( m_socket->TLSCreateSSL())
				{
					bRet = true;
				}
			}

			return bRet;
		}

		bool CTcpClient::EnableTLS(const std::string& Certificate, const std::string& PrivateKey, const std::string& CaCertificate)
		{
			bool bRet = false;

			if (m_socket->TLSCreateContext(false))
			{
				if (m_socket->TLSLoadCertificateAndKey(Certificate, PrivateKey))
				{
					if (CaCertificate.empty())
					{
						if (m_socket->TLSEnableMutualAuthentication({ DevLib::IO::TLS::GetRootCA() }))
						{
							if (m_socket->TLSCreateSSL())
							{
								bRet = true;
							}
						}
					}
					else
					{
						if (m_socket->TLSEnableMutualAuthentication({ CaCertificate }))
						{
							if (m_socket->TLSCreateSSL())
							{
								bRet = true;
							}
						}
					}
				}
			}

			return bRet;
		}

		bool CTcpClient::EnableTLSFile(const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& CaCertificateFile)
		{
			bool bRet = false;

			if (m_socket->TLSCreateContext(false))
			{
				if (m_socket->TLSLoadCertificateAndKeyFile(CertificateFile, PrivateKeyFile))
				{
					if (CaCertificateFile.empty())
					{
						if (m_socket->TLSEnableMutualAuthentication({ DevLib::IO::TLS::GetRootCA() }))
						{
							if (m_socket->TLSCreateSSL())
							{
								bRet = true;
							}
						}
					}
					else
					{
						if( m_socket->TLSEnableMutualAuthenticationFile(CaCertificateFile) )
						{
							if (m_socket->TLSCreateSSL())
							{
								bRet = true;
							}
						}
					}
				}
			}

			return bRet;
		}

		bool CTcpClient::IsEnableTLS()
		{
			return m_socket->TLSIsCreatedSSL();
		}

		void CTcpClient::OnTimerAutoConnection()
		{
			if (m_csConnection.TryLock())
			{
				if (m_bCreatedClient)
				{
					if (!m_socket->IsCreated())
					{
						m_socket->Create(SOCK_TYPE_TCP);
					}

					if (!SocketUtility::IsConnected(*m_socket))
					{
						const bool bConnect = m_socket->Connect(m_serverPort, m_serverIP);

#if defined(_MSC_VER)
						if (GetLastError() == WSAENOTSOCK && m_bCreatedClient)
#elif defined(__linux__) // Linux
						if (m_socket->GetSocketHandle() == -1 && m_bCreatedClient)
#endif
						{
							m_socket->Destroy();

							if (m_socket->Create(SOCK_TYPE_TCP))
							{
								m_socket->Connect(m_serverPort, m_serverIP);
							}
						}

						if (bConnect)
						{
							// TLS
							if( IsEnableTLS() )
							{
								if( m_socket->TLSBind() )
								{
									if( m_socket->TLSConnect() )
									{
										m_callbackConnection(SocketUtility::addrToIP(m_socket->GetAddressRemote()), SocketUtility::addrToPort(m_socket->GetAddressRemote()));
									}
								}
							}
							else
							{
								m_callbackConnection(SocketUtility::addrToIP(m_socket->GetAddressRemote()), SocketUtility::addrToPort(m_socket->GetAddressRemote()));
							}

							m_socket.RegisterCallbackSocketState(&CTcpClient::OnServerSate, this);
							m_socket->StartWatchDog();
							m_timerConnection.StopTimer();
						}
					}
				}
				else
				{
					m_timerConnection.StopTimer();
				}
				m_csConnection.UnLock();
			}
		}

		void CTcpClient::OnDataServer(CSocket& sock, const void_ptr pData, const int32_t size) const
		{
			m_callbackReceiveData(std::forward<CSocket&>(sock), SocketUtility::addrToIP(sock.GetAddressRemote()), SocketUtility::addrToPort(sock.GetAddressRemote()), pData, size);
		}

		void CTcpClient::OnServerSate(CSocket& server, const uint32_t state)
		{
			switch (state)
			{
			case 0:
			case 104: // Linux
			case 10054:
				server.StopWatchDog();
				server.Destroy();
				m_callbackDisConnection(SocketUtility::addrToIP(server.GetAddressRemote()), SocketUtility::addrToPort(server.GetAddressRemote()));

				// TLS
				if(IsEnableTLS())
				{
					m_socket->TLSShutdownSSL();
					m_socket->TLSCreateSSL();
				}

				m_timerConnection.StartTimer(m_timeAutoConnection, &CTcpClient::OnTimerAutoConnection, this);
				break;
			default:
				break;
			}

		}
	}
}
