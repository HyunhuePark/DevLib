#include "../../../include/Routine/WebServer/CWebServer.hpp"

#include "../../../include/Utility/String.hpp"
#include "../../../include/Routine/WebServer/DevLibHTTPUtility.hpp"
#include "../../../include/IO/DevLibSocketUtility.hpp"

namespace DevLib {
	namespace Routine
	{
		namespace HTTP
		{
			CWebServer::CWebServer()
			{
				m_server.RegisterCallbackReceiveData(&CWebServer::OnReceiveData, this);
				m_server.RegisterCallbackConnection(&CWebServer::OnHTTPConnection, this);
				m_server.RegisterCallbackDisConnection(&CWebServer::OnHTTPDisConnection, this);
			}

			bool CWebServer::HTTPOpen(uint16_t port, const std::string& bindIP, const uint32_t listenCount)
			{
				m_timerKeepAlive.StartTimer(10000, &CWebServer::OnTimerCheckKeepAlive, this);
				return m_server.CreateTcpServer(port, bindIP, true, listenCount);
			}

			bool CWebServer::HTTPSOpen(uint16_t port, const std::string& CertificateFile, const std::string& PrivateKeyFile, const std::string& bindIP, uint32_t listenCount)
			{
				bool bRet = false;
				m_timerKeepAlive.StartTimer(10000, &CWebServer::OnTimerCheckKeepAlive, this);

				if(m_server.EnableTLSFile(CertificateFile, PrivateKeyFile))
				{
					bRet = m_server.CreateTcpServer(port, bindIP, true, listenCount);
				}

				return bRet;
			}

			void CWebServer::OnHTTPConnection(const std::string& ip, uint16_t port)
			{
				m_lockClientStream.Lock();
				if (m_keepAlive[ip].find(port) == m_keepAlive[ip].end())
				{
					// Add Client StreamBuffer
					const DevLib::CTime connectionTime;
					m_keepAlive[ip][port] = connectionTime.GetTimestamp() + 60;	// default 1min
				}
				m_lockClientStream.UnLock();

				if (m_callbackConnection) m_callbackConnection(ip, port);
			}

			void CWebServer::OnHTTPDisConnection(const std::string& ip, uint16_t port)
			{
				m_lockClientStream.Lock();
				if (m_keepAlive.find(ip) != m_keepAlive.end())
				{
					if (m_keepAlive[ip].find(port) != m_keepAlive[ip].end())
					{
						// Remove Client StreamBuffer
						m_keepAlive[ip].erase(port);
					}
				}
				m_lockClientStream.UnLock();

				if (m_callbackDisConnection) m_callbackDisConnection(ip, port);
			}

			void CWebServer::OnReceiveData(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)
			{
				std::string strCommand(static_cast<char*>(pData));

				// printf("%s", strCommand.c_str());

				//// ToLower
				//std::transform(strCommand.begin(), strCommand.end(), strCommand.begin(), std::tolower);
				
				// Parse Line
				std::istringstream inputStream(strCommand);

				// 1. Star Line ( Method URL Version )
				std::string startLine;
				std::getline(inputStream, startLine, '\n');

				// parse Start Line
				std::istringstream startLineStream(startLine);

				std::string strMethod;
				std::string strURL;
				std::string strVersion;
				std::getline(startLineStream, strMethod, ' ');
				std::getline(startLineStream, strURL, ' ');
				std::getline(startLineStream, strVersion, ' ');

				strURL = UTF8ToMultiByte(HTTP::DecodeURI(strURL));

				// 2. Header Info
				std::map<std::string, std::string> headerInfo;
				std::string info;
				while (std::getline(inputStream, info, '\n'))
				{
					std::istringstream headerInfoStream(info);

					std::string attributeName;
					std::string attributeValue;
					std::getline(headerInfoStream, attributeName, ':');
					std::getline(headerInfoStream, attributeValue, '\r');
					attributeValue.erase(0, attributeValue.find_first_not_of(' '));
					attributeValue.erase(attributeValue.find_last_not_of(' ') + 1);
					attributeValue.erase(attributeValue.find_last_not_of('\r') + 1);
					attributeValue.erase(attributeValue.find_last_not_of('\n') + 1);

					if (attributeName == "\r") // Find End
					{
						break;
					}

					headerInfo[attributeName] = attributeValue;
				}


				// 3. Payload
				std::string payload;

				auto strSize = headerInfo["Content-Length"];

				if(!strSize.empty())
				{
					std::getline(inputStream, payload);

					const size_t nPayloadSize = std::stoi(strSize);
					auto nReadSize = payload.length();
					payload.resize(nPayloadSize);
					auto pDst = payload.data();
					
					int ReTryCount = 5; // 5s
					while( nReadSize < nPayloadSize )
					{
						// printf("Remain Payload Size : %ld / %ld\n", nReadSize, nPayloadSize);
						if( client.WaitForRecv(1000) )
						{
							auto nRecv = client.Recv(&pDst[nReadSize], nPayloadSize - nReadSize);
							if( nRecv >= 0 )
							{
								nReadSize+=nRecv;
							}
						}

						if( ReTryCount-- < 1 )
						{
							return;
						}
					}
				}

				// Type
				if (strMethod == "GET" && m_callbackRequestGet)
				{
					m_callbackRequestGet(client, headerInfo, strURL);
				}
				else if (strMethod == "POST" && m_callbackRequestPost)
				{
					payload = UTF8ToMultiByte(HTTP::DecodeURI(payload));

					m_callbackRequestPost(client, headerInfo, strURL, payload);
				}
				else if (strMethod == "PUT" && m_callbackRequestPut)
				{
					payload = UTF8ToMultiByte(HTTP::DecodeURI(payload));

					m_callbackRequestPut(client, headerInfo, strURL, payload);
				}
				else if (strMethod == "DELETE" && m_callbackRequestDelete)
				{
					m_callbackRequestDelete(client, headerInfo, strURL);
				}
				else if (strMethod == "OPTIONS" && m_callbackRequestOptions)
				{
					m_callbackRequestOptions(client, headerInfo, strURL);
				}
				else if (strMethod == "TRACE" && m_callbackRequestTrace)
				{
					m_callbackRequestTrace(client, headerInfo, strURL);
				}
				else
				{
					if (m_callbackRequestEx)
					{
						m_callbackRequestEx(client, startLine);
					}
				}

				// 4. Remain Request
				std::string strRemain;
				std::getline(inputStream, strRemain);
				if(!strRemain.empty())
				{
					OnReceiveData(client, ip,  port, strRemain.data(), static_cast<int32_t>(strRemain.size()));
				}

				// 5. Option 
				auto OptConnection = headerInfo["Connection"];
				if(OptConnection.empty())
				{
					OptConnection = headerInfo["connection"];
				} 

				if(!OptConnection.empty())
				{
					if( OptConnection == "Keep-Alive" || OptConnection == "keep-alive")
					{
						m_lockClientStream.Lock();
						if (m_keepAlive.find(ip) != m_keepAlive.end())
						{
							if (m_keepAlive[ip].find(port) != m_keepAlive[ip].end())
							{
								(void)client.SetSockKeepAlive(true);

								// Update Current Time
								DevLib::CTime curTime;

								m_keepAlive[ip][port] = curTime.GetTimestamp() + 600;
							}
						}
						m_lockClientStream.UnLock();
					}
				}
			}

			void CWebServer::OnTimerCheckKeepAlive()
			{
				// printf("OnTimerCheckKeepAlive - Client Count : %ld\n",m_server.GetClientInfo().size() );
				const DevLib::CTime curTime;

				m_lockClientStream.Lock();

				// find Over Time
				const std::vector<std::pair<std::string, uint16_t>> closeList;
				for (auto &ipGroup : m_keepAlive)
				{
					for (const auto &client : ipGroup.second)
					{
						if( client.second < curTime.GetTimestamp() )
						{
							// client Destory
							m_server.DisconnectClient(ipGroup.first, client.first);
						}
					}
				}

				for( auto & socket : closeList)
				{
					m_server.DisconnectClient(socket.first, socket.second);

					// // list Remove
					m_keepAlive[socket.first].erase(socket.second);
				}

				m_lockClientStream.UnLock();
			}
		}
	}
}
