#include "../../include/IO/CTcpServer.hpp"
#include "../../include/IO/DevLibSocketUtility.hpp"

#include <openssl/ssl.h>

namespace DevLib
{
	namespace IO
	{

		CTcpServer::CTcpServer()
			: m_sizeBuffer(8192)
		{
			RegisterCallbackConnection(&CTcpServer::OnConnection, this);
			RegisterCallbackDisConnection(&CTcpServer::OnDisConnection, this);
			RegisterCallbackReceiveData(&CTcpServer::OnReceiveData, this);
		}

		CTcpServer::~CTcpServer()
		{
			m_socketListen.StopWatchDog();
			m_threadListen.StopThread();
			m_threadListen.WaitForEndThread();
		}
		
		bool CTcpServer::CreateTcpServer(uint16_t port, const std::string& bindIP /*= std::string()*/, bool bBlockMode /*= true*/, const int32_t nListen /*= 5*/)
		{
			bool bRet = m_socketListen.Create(SOCK_TYPE_TCP);

			if (bRet)
			{
				bRet = m_socketListen.Bind(port, bindIP);

				if (bRet)
				{
					bRet = m_socketListen.Listen(nListen);

					if (bRet)
					{
						m_threadListen.StartThread(&CTcpServer::WatchDogConnection, this);
					}
					else
					{
						m_socketListen.Destroy();
					}
				}
				else
				{
					m_socketListen.Destroy();
				}
			}

			return bRet;
		}

		void CTcpServer::CloseTcpServer()
		{
			m_socketListen.Destroy();

			m_threadListen.StopThread();
			m_threadListen.WaitForEndThread(1000);
			m_listClient.Lock();
			m_listClient->clear();
			m_listClient.UnLock();
		}

		void CTcpServer::SetBufferSize(const size_t sizeBuffer /*= 8192*/)
		{
			m_sizeBuffer = sizeBuffer;

			// safe delete client
			m_listClient.Lock();

			for (auto& itr : *m_listClient)
			{
				if (!itr.IsCreated())
				{
					itr.SetBufferSize(sizeBuffer);
					break;
				}
			}

			m_listClient.UnLock();
		}

		int32_t CTcpServer::Send(void_const_ptr pSrc, size_t size, int32_t nFlag)
		{
			m_listClient.Lock();

			for (auto& itr : *m_listClient)
			{
				if (itr.IsCreated())
				{
					(void)itr.GetSocket().Send(pSrc, size, nFlag);
				}
			}

			m_listClient.UnLock();

			return static_cast<int32_t>(size);
		}

		int32_t CTcpServer::SendTo(const std::string& ip, uint16_t port, void_const_ptr pSrc, size_t size, int32_t nFlag)
		{
			int32_t ret = 0;
			m_listClient.Lock();

			for (auto& itr : *m_listClient)
			{
				const auto clientIP = SocketUtility::addrToIP(itr->GetAddressRemote());
				const auto clientPort = SocketUtility::addrToPort(itr->GetAddressRemote());

				if (clientIP == ip && port == clientPort)
				{
					if( itr.GetSocket().WaitForWritable() )
					{
						ret = itr.GetSocket().Send(pSrc, size, nFlag);
					}
				}
			}

			m_listClient.UnLock();

			return ret;
		}

		std::vector<CSocket> CTcpServer::GetClientInfo()
		{
			std::vector<CSocket> info;

			m_listClient.Lock();
			for (auto& val : *m_listClient)
			{
				info.push_back(val.GetSocket());
			}
			m_listClient.UnLock();

			return info;
		}

		std::optional<CSocket>  CTcpServer::GetClientSocket(const std::string& ip, const uint16_t port)
		{
			std::optional<CSocket> bRet;

			m_listClient.Lock();
			for (auto& val : *m_listClient)
			{
				if( val.GetSocket().GetAddressRemoteIP() == ip && val.GetSocket().GetAddressRemotePort() == port)
				{
					bRet = val.GetSocket();
					break;
				}
			}
			m_listClient.UnLock();
			
			return bRet;
		}

		bool CTcpServer::SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count) const
		{
			bool bRet = false;
			if (m_socketListen.IsCreated())
			{
				bRet = DevLib::IO::SocketUtility::SetSockKeepAlive(m_socketListen.GetSocketHandle(), true, second, intervalSecond, count);
			}
			return bRet;
		}

		bool CTcpServer::EnableTLS(const std::string& Certificate, const std::string& PrivateKey)
		{
			bool bRet = false;

			if(  m_socketListen.TLSCreateContext(true) )
			{
				bRet = m_socketListen.TLSLoadCertificateAndKey(Certificate, PrivateKey);
			}

			return bRet;
		}

		bool CTcpServer::EnableTLSFile(const std::string& CertificateFile, const std::string& PrivateKeyFile)
		{
			bool bRet = false;

			if (m_socketListen.TLSCreateContext(true))
			{
				bRet = m_socketListen.TLSLoadCertificateAndKeyFile(CertificateFile, PrivateKeyFile);
			}

			return bRet;
		}

		bool CTcpServer::IsEnableTLS() const
		{
			return m_socketListen.TLSIsCreatedContext();
		}

		bool CTcpServer::EnableMTLS(const std::string& Certificate)
		{
			return m_socketListen.TLSEnableMutualAuthentication({ Certificate });
		}

		bool CTcpServer::EnableMTLSFile(const std::string& CertificateFile)
		{
			return m_socketListen.TLSEnableMutualAuthenticationFile(CertificateFile);
		}

		void CTcpServer::DisconnectClient(const std::string &ip, const uint16_t port)
		{
			m_listClient.Lock();

			for (auto itr = m_listClient->begin(); itr != m_listClient->end(); ++itr)
			{
				if( (*itr)->GetAddressRemoteIP() == ip && (*itr)->GetAddressRemotePort() == port)
				{
					if (itr->IsCreated())
					{
						 (*itr)->Destroy();
					}
					break;
				}
			}

			m_listClient.UnLock();
		}

		void CTcpServer::WatchDogConnection()
		{
			while (m_threadListen.IsRunThread())
			{
				if (m_socketListen.WaitForRecv(100))
				{
					std::reverse_iterator<std::list<CSocketReceiver>::iterator> curClient;
					m_listClient.Lock();
					m_listClient->emplace_back();

					bool bAccept = m_socketListen.Accept(m_listClient->rbegin()->GetSocket());

					if (m_callbackAccept)
					{
						bAccept = m_callbackAccept(m_listClient->rbegin()->GetSocket());
					}

					if (bAccept)
					{
						curClient = m_listClient->rbegin();

						curClient->RegisterCallbackReceivedData(&CTcpServer::OnDataClient, this);
						curClient->RegisterCallbackSocketState(&CTcpServer::OnClientSocketState, this);
						curClient->SetBufferSize(m_sizeBuffer);

						// TLS
						if (IsEnableTLS())
						{
							curClient->GetSocket().TLSCreateSSL(m_socketListen.TLSGetContext());

							if (curClient->GetSocket().TLSAccept())
							{								

							}
							else
							{
								bAccept = false;
								m_listClient->erase(--m_listClient->end());
							}
						}
					}
					else
					{
						m_listClient->erase(--m_listClient->end());
					}

					m_listClient.UnLock();

					// callback connected
					if (bAccept)
					{
						// const client
						auto& client = curClient->GetSocket();
						m_callbackConnection(client.GetAddressRemoteIP(), client.GetAddressRemotePort());
						client.StartWatchDog();
					}
				}
				else
				{
					// safe delete client
					m_listClient.Lock();

					for (auto itr = m_listClient->begin(); itr != m_listClient->end(); )
					{
						if ((*itr)->IsDestroyed())
						{
							// m_callbackDisConnection((*itr)->GetAddressRemoteIP(), (*itr)->GetAddressRemotePort());
							// (*itr)->Destroy();
							itr = m_listClient->erase(itr);
						}
						else
						{
							++itr;
						}
					}

					m_listClient.UnLock();
				}
			}
		}

		void CTcpServer::OnDataClient(CSocket& client, void_ptr pData, const int32_t size) const
		{
			m_callbackReceiveData(std::forward<CSocket&>(client), client.GetAddressRemoteIP(), client.GetAddressRemotePort(), pData, size);
		}

		void CTcpServer::OnClientSocketState(CSocket& client, uint32_t state) const
		{
			switch (state)
			{
			case 0:
			case 9:   // CLOSE_WAIT
			case 11:  // TIME_WAIT
			case 104: // Linux
			case 110: // Connection Time Out
			case 10054:
				m_callbackDisConnection(client.GetAddressRemoteIP(), client.GetAddressRemotePort());
				client.Destroy();
				break;
			default:
			printf("OnClientSocketState : %s ( %d ) : %d\n", client.GetAddressRemoteIP().c_str(), client.GetAddressRemotePort(), state);
				break;
			}
		}
	}
}
