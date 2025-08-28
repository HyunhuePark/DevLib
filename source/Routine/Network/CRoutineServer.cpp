#include "../../../include/Routine/Network/CRoutineServer.hpp"

#include <cryptopp/aes.h>
#include "../../../include/Routine/Crypto/CryptoUtility.hpp"

#include "../../../include/Routine/Network/Message/RoutineMessageType.hpp"

#include "../../../include/IO/DevLibSocketUtility.hpp"

namespace DevLib {
	namespace Routine
	{
		CRoutineServer::CRoutineServer()
		{
			m_sockTCP.RegisterCallbackConnection(&CRoutineServer::OnConnectedClient, this);
			m_sockTCP.RegisterCallbackDisConnection(&CRoutineServer::OnDisConnectedClient, this);
			m_sockTCP.RegisterCallbackReceiveData(&CRoutineServer::OnReceivedTCP, this);

			m_sockTCP.SetBufferSize(sizeof(RoutineMessageHeader));

			m_sockUDP.RegisterCallbackReceive(&CRoutineServer::OnReceivedUDP, this);

			m_workProcessQueue.CreateAsync("CRoutineServer::ProcessQueue", &CRoutineServer::OnProcessQueue, this);
		}

		bool CRoutineServer::CreateServer(uint16_t port, const std::string& bind, bool bBlockMode, const std::vector<uint8_t>& key)
		{
			m_cryptoKey = key;

			m_messageParser.Create();
			m_sockUDP.Create(IO::SOCK_TYPE_UDP);
			m_sockUDP.Bind(port);
			m_sockUDP.StartWatchDog();

			return m_sockTCP.CreateTcpServer(port, bind, bBlockMode);
		}

		void CRoutineServer::CloseServer()
		{
			m_sockUDP.Destroy();
			m_sockTCP.CloseTcpServer();
			m_cryptoKey.clear();

			m_infoClient->clear();
		}

		void CRoutineServer::WriteMessageTCP(uint32_t messageID, const uint8_t* pData, size_t size)
		{
			if (!m_cryptoKey.empty())
			{
				std::vector<uint8_t> encData;
				
				const auto iv = Crypto::GeneratorInitialVector();
				if (Crypto::EncryptionAES(m_cryptoKey, iv, pData, size, encData))
				{
					m_sendBuffer.resize(sizeof(RoutineMessageHeader) + CryptoPP::AES::BLOCKSIZE + encData.size());
					if (m_sendBuffer.size() == sizeof(RoutineMessageHeader) + CryptoPP::AES::BLOCKSIZE + encData.size())
					{
						auto* pTcpHeader = reinterpret_cast<RoutineMessageHeader*>(m_sendBuffer.data());

						pTcpHeader->MessageID = messageID;
						pTcpHeader->MessageLength = static_cast<uint32_t>(CryptoPP::AES::BLOCKSIZE + encData.size());
						if (!encData.empty())
						{
							auto* pTcpBuffer = &m_sendBuffer[sizeof(RoutineMessageHeader)];
							memcpy(pTcpBuffer, iv.data(), iv.size()); // copy IV
							memcpy(&pTcpBuffer[iv.size()], encData.data(), encData.size()); // copy EncPayload
						}
					}
				}
			}
			else
			{
				m_sendBuffer.resize(sizeof(RoutineMessageHeader) + size);
				if (m_sendBuffer.size() == sizeof(RoutineMessageHeader) + size)
				{
					auto* pTcpHeader = reinterpret_cast<RoutineMessageHeader*>(m_sendBuffer.data());

					pTcpHeader->MessageID = messageID;
					pTcpHeader->MessageLength = static_cast<uint32_t>(size);
					if (size)
					{
						auto* pTcpBuffer = &m_sendBuffer[sizeof(RoutineMessageHeader)];
						memcpy(pTcpBuffer, pData, size);
					}
				}
			}

			size_t nRemain = m_sendBuffer.size();
			while (nRemain > 0)
			{
				const int32_t nSend = m_sockTCP.Send(&m_sendBuffer[m_sendBuffer.size() - nRemain], nRemain);
				if (nSend > 0)
				{
					nRemain -= nSend;
				}
				else
				{
					break;
				}
			}

			m_sendBuffer.clear();
		}

		void CRoutineServer::WriteMessageUDP(uint32_t messageID, const uint8_t* pData, size_t size)
		{
			if (!m_cryptoKey.empty())
			{
				std::vector<uint8_t> encData;

				const auto iv = Crypto::GeneratorInitialVector();
				if (Crypto::EncryptionAES(m_cryptoKey, iv, pData, size, encData))
				{
					m_sendBuffer.resize(CryptoPP::AES::BLOCKSIZE + encData.size());
					if (m_sendBuffer.size() == CryptoPP::AES::BLOCKSIZE + encData.size())
					{
						if (!encData.empty())
						{
							auto* pSendBuffer = m_sendBuffer.data();
							memcpy(pSendBuffer, iv.data(), iv.size()); // copy IV
							memcpy(&pSendBuffer[iv.size()], encData.data(), encData.size()); // copy EncPayload
						}
					}
				}
			}
			else
			{
				m_sendBuffer.resize(size);
				auto* pSendBuffer = m_sendBuffer.data();
				memcpy(pSendBuffer, pData, size); 
			}


			std::vector<uint8_t> udpPacket(UDP_MAX_PACKET_SIZE + sizeof(RoutineUDPHeader));
			const size_t totalSize = (sizeof(RoutineMessageHeader) + m_sendBuffer.size());
			size_t nCount = totalSize / UDP_MAX_PACKET_SIZE;	// Total Size = Routine header + RawData

			auto* udpHeader = reinterpret_cast<RoutineUDPHeader*>(udpPacket.data());
			auto* pUdpBuffer = &udpPacket[sizeof(RoutineUDPHeader)];

			RoutineMessageHeader msgHeader;
			msgHeader.MessageID = messageID;
			msgHeader.MessageLength = static_cast<uint32_t>(m_sendBuffer.size());

			if (nCount > 0)
			{
				// Message Header
				memcpy(pUdpBuffer, &msgHeader, sizeof(RoutineMessageHeader));

				// Payload
				size_t srcPos = 0;
				udpHeader->bFragment = 1;
				udpHeader->PacketSize = UDP_MAX_PACKET_SIZE;
				memcpy(&pUdpBuffer[sizeof(RoutineMessageHeader)], &m_sendBuffer[srcPos], UDP_MAX_PACKET_SIZE - sizeof(RoutineMessageHeader));
				srcPos = UDP_MAX_PACKET_SIZE - sizeof(RoutineMessageHeader);

				if (!m_infoClient->empty())
				{
					for (const auto& ip : *m_infoClient)
					{
						for (const auto& port : ip.second)
						{
							m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip.first, port.first);
						}
					}
				}

				do
				{
					udpHeader->FragmentOffset += udpHeader->PacketSize;

					if (nCount == 1) // Last Data
					{
						udpHeader->bFragment = 0;
						udpHeader->PacketSize = totalSize % UDP_MAX_PACKET_SIZE;
						memcpy(pUdpBuffer, &m_sendBuffer[srcPos], udpHeader->PacketSize);
					}
					else
					{
						udpHeader->bFragment = 1;
						udpHeader->PacketSize = UDP_MAX_PACKET_SIZE;
						memcpy(pUdpBuffer, &m_sendBuffer[srcPos], udpHeader->PacketSize);
						srcPos += udpHeader->PacketSize;
					}

					if (!m_infoClient->empty())
					{
						for (const auto& ip : *m_infoClient)
						{
							for (const auto& port : ip.second)
							{
								if (m_sockUDP.WaitForWritable(10)) // default 10 ms
								{
									m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip.first, port.first);
								}
								else
								{
									m_sendBuffer.clear();
									return;
								}
							}
						}
					}
				} while (--nCount);

			}
			else
			{
				// Message Header
				memcpy(pUdpBuffer, &msgHeader, sizeof(RoutineMessageHeader));

				// Payload
				udpHeader->bFragment = 0;
				udpHeader->PacketSize = static_cast<uint16_t>(sizeof(RoutineMessageHeader) + m_sendBuffer.size());
				memcpy(&pUdpBuffer[sizeof(RoutineMessageHeader)], m_sendBuffer.data(), m_sendBuffer.size());

				if (!m_infoClient->empty())
				{
					for (const auto& ip : *m_infoClient)
					{
						for (const auto& port : ip.second)
						{
							m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip.first, port.first);
						}
					}
				}
			}

			m_sendBuffer.clear();
		}

		void CRoutineServer::WriteMessageTCPTo(const string_t& ip, const uint16_t port, uint32_t messageID, const uint8_t* pData, size_t size)
		{
			if (!m_cryptoKey.empty())
			{
				std::vector<uint8_t> encData;

				const auto iv = Crypto::GeneratorInitialVector();
				if (Crypto::EncryptionAES(m_cryptoKey, iv, pData, size, encData))
				{
					m_sendBuffer.resize(sizeof(RoutineMessageHeader) + CryptoPP::AES::BLOCKSIZE + encData.size());
					if (m_sendBuffer.size() == sizeof(RoutineMessageHeader) + CryptoPP::AES::BLOCKSIZE + encData.size())
					{
						auto* pTcpHeader = reinterpret_cast<RoutineMessageHeader*>(m_sendBuffer.data());

						pTcpHeader->MessageID = messageID;
						pTcpHeader->MessageLength = static_cast<uint32_t>(CryptoPP::AES::BLOCKSIZE + encData.size());
						pTcpHeader->Crypto = ROUTINE_CRYPTO_ENCRYPTED;

						if (!encData.empty())
						{
							auto* pTcpBuffer = &m_sendBuffer[sizeof(RoutineMessageHeader)];
							memcpy(pTcpBuffer, iv.data(), iv.size()); // copy IV
							memcpy(&pTcpBuffer[iv.size()], encData.data(), encData.size()); // copy EncPayload
						}
					}
				}
			}
			else
			{
				m_sendBuffer.resize(sizeof(RoutineMessageHeader) + size);
				if (m_sendBuffer.size() == sizeof(RoutineMessageHeader) + size)
				{
					auto* pTcpHeader = reinterpret_cast<RoutineMessageHeader*>(m_sendBuffer.data());

					pTcpHeader->MessageID = messageID;
					pTcpHeader->MessageLength = static_cast<uint32_t>(size);
					pTcpHeader->Crypto = ROUTINE_CRYPTO_UNENCRYPTED;

					if (size)
					{
						auto* pTcpBuffer = &m_sendBuffer[sizeof(RoutineMessageHeader)];
						memcpy(pTcpBuffer, pData, size);
					}
				}
			}

			size_t nRemain = m_sendBuffer.size();
			
			while (nRemain > 0)
			{
				const int32_t nSend = m_sockTCP.SendTo(ip, port, &m_sendBuffer[m_sendBuffer.size() - nRemain], nRemain);
				if (nSend > 0)
				{
					nRemain -= nSend;
				}
				else
				{
					break;
				}
			}

			m_sendBuffer.clear();
		}

		void CRoutineServer::WriteMessageUDPTo(const string_t& ip, const uint16_t port, uint32_t messageID, const uint8_t* pData, size_t size)
		{
			RoutineMessageHeader msgHeader;
			msgHeader.MessageID = messageID;

			if (!m_cryptoKey.empty())
			{
				msgHeader.Crypto = ROUTINE_CRYPTO_ENCRYPTED;

				std::vector<uint8_t> encData;

				const auto iv = Crypto::GeneratorInitialVector();
				if (Crypto::EncryptionAES(m_cryptoKey, iv, pData, size, encData))
				{
					m_sendBuffer.resize(CryptoPP::AES::BLOCKSIZE + encData.size());
					if (m_sendBuffer.size() == CryptoPP::AES::BLOCKSIZE + encData.size())
					{
						if (!encData.empty())
						{
							auto* pSendBuffer = m_sendBuffer.data();
							memcpy(pSendBuffer, iv.data(), iv.size()); // copy IV
							memcpy(&pSendBuffer[iv.size()], encData.data(), encData.size()); // copy EncPayload
						}
					}
				}
			}
			else
			{
				msgHeader.Crypto = ROUTINE_CRYPTO_UNENCRYPTED;

				m_sendBuffer.resize(size);
				auto* pSendBuffer = m_sendBuffer.data();
				memcpy(pSendBuffer, pData, size);
			}

			msgHeader.MessageLength = static_cast<uint32_t>(m_sendBuffer.size());


			std::vector<uint8_t> buff(UDP_MAX_PACKET_SIZE + sizeof(RoutineUDPHeader));
			const size_t totalSize = (sizeof(RoutineMessageHeader) + m_sendBuffer.size());
			size_t nCount = totalSize / UDP_MAX_PACKET_SIZE;	// Total Size = Routine header + RawData

			auto* udpHeader = reinterpret_cast<RoutineUDPHeader*>(buff.data());
			auto* pUdpBuffer = &buff[sizeof(RoutineUDPHeader)];
			
			if (nCount > 0)
			{
				// Message Header
				memcpy(pUdpBuffer, &msgHeader, sizeof(RoutineMessageHeader));

				// Payload
				size_t srcPos = 0;
				udpHeader->bFragment = 1;
				udpHeader->PacketSize = UDP_MAX_PACKET_SIZE;
				memcpy(&pUdpBuffer[sizeof(RoutineMessageHeader)], &m_sendBuffer[srcPos], UDP_MAX_PACKET_SIZE - sizeof(RoutineMessageHeader));
				srcPos = UDP_MAX_PACKET_SIZE - sizeof(RoutineMessageHeader);

				if (!m_infoClient->empty())
				{
					m_sockUDP.SendTo(buff.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip, port);
				}

				do
				{
					udpHeader->FragmentOffset += udpHeader->PacketSize;

					if (nCount == 1) // Last Data
					{
						udpHeader->bFragment = 0;
						udpHeader->PacketSize = totalSize % UDP_MAX_PACKET_SIZE;
						memcpy(pUdpBuffer, &m_sendBuffer[srcPos], udpHeader->PacketSize);
					}
					else
					{
						udpHeader->bFragment = 1;
						udpHeader->PacketSize = UDP_MAX_PACKET_SIZE;
						memcpy(pUdpBuffer, &m_sendBuffer[srcPos], udpHeader->PacketSize);
						srcPos += udpHeader->PacketSize;
					}

					if (m_sockUDP.WaitForWritable(10)) // default 10 ms
					{
						m_sockUDP.SendTo(buff.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip, port);
					}
					else
					{
						break;
					}

				} while (--nCount);
			}
			else
			{
				// Message Header
				memcpy(pUdpBuffer, &msgHeader, sizeof(RoutineMessageHeader));

				// Payload
				udpHeader->bFragment = 0;
				udpHeader->PacketSize = static_cast<uint16_t>(sizeof(RoutineMessageHeader) + size);
				memcpy(&pUdpBuffer[sizeof(RoutineMessageHeader)], m_sendBuffer.data(), size);

				m_sockUDP.SendTo(buff.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, ip, port);
			}

			m_sendBuffer.clear();
		}

		bool CRoutineServer::SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count) const
		{
			return m_sockTCP.SetKeepAliveTime(second, intervalSecond, count);
		}

		void CRoutineServer::OnConnectedClient(const std::string& ip, uint16_t port)
		{
			m_infoClient.Lock();

			auto &clientIP = m_infoClient.get()[ip];
			clientIP[port];

			if (m_callbackConnection) m_callbackConnection(ip, port);

			m_infoClient.UnLock();
		}

		void CRoutineServer::OnDisConnectedClient(const std::string& ip, uint16_t port)
		{
			m_infoClient.Lock();
			if (m_infoClient->find(ip) != m_infoClient->end())
			{
				auto& clientInfo = m_infoClient.get()[ip];

				if (clientInfo.find(port) != clientInfo.end())
				{
					clientInfo.erase(port);
					if (m_callbackDisConnection) m_callbackDisConnection(ip, port);
				}

				if (clientInfo.empty()) m_infoClient->erase(ip);
			}


			m_infoClient.UnLock();
		}

		void CRoutineServer::OnReceivedTCP(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)
		{
			if (size == sizeof(RoutineMessageHeader))
			{
				const auto pHeader = static_cast<RoutineMessageHeader *>(pData);
				auto tcpBuffer = std::make_shared<std::vector<uint8_t>>(pHeader->MessageLength);
				size_t nBuffPos = 0;

				while (pHeader->MessageLength - nBuffPos)
				{
					const int32_t nRecvSize = client.Recv(&tcpBuffer->data()[nBuffPos], pHeader->MessageLength - nBuffPos);
					if (nRecvSize <= 0)
						return;
					nBuffPos += nRecvSize;
				}

				if (pHeader->Crypto == ROUTINE_CRYPTO_ENCRYPTED)
				{
					if (!m_cryptoKey.empty())
					{
						const auto decBuffer = std::make_shared<std::vector<uint8_t>>();
						const auto pRawPtr = tcpBuffer->data();

						const std::vector iv(pRawPtr, pRawPtr + CryptoPP::AES::BLOCKSIZE);

						if (Crypto::DecryptionAES(m_cryptoKey, iv, &tcpBuffer->data()[CryptoPP::AES::BLOCKSIZE], tcpBuffer->size() - CryptoPP::AES::BLOCKSIZE, *decBuffer))
						{
							tcpBuffer = decBuffer;
							m_messageParser.ReceivedMessage(pHeader->MessageID, ip, port, tcpBuffer);
						}
					}	
				}
				else
				{
					m_messageParser.ReceivedMessage(pHeader->MessageID, ip, port, tcpBuffer);
				}
			}
		}

		void CRoutineServer::OnReceivedUDP(IO::CSocket& sock)
		{
			PacketUDP packet;
			std::get<2>(packet) = std::make_shared<std::vector<uint8_t>>(65535);

			if (sock.RecvFrom(
				std::get<2>(packet).get()->data(), std::get<2>(packet).get()->size(),
				std::get<0>(packet), std::get<1>(packet)) > 0)
			{
				m_packetsUDP.Lock();
				m_packetsUDP->emplace_back(packet);
				m_packetsUDP.UnLock();

				m_workProcessQueue.Work();
			}
		}

		void CRoutineServer::OnProcessQueue()
		{
			m_packetsUDP.Lock();
			const auto vPackets = std::move(m_packetsUDP.get());
			m_packetsUDP.UnLock();

			for (const auto& PacketData : vPackets)
			{
				const auto ip = std::get<0>(PacketData);
				const auto port = std::get<1>(PacketData);
				const auto packet = std::get<2>(PacketData);

				if (m_infoClient->find(ip) != m_infoClient->end())
				{
					auto& foundIP = m_infoClient.get()[ip];
					if (foundIP.find(port) != foundIP.end())
					{
						const auto pRawPtr = packet->data();
						const auto pHeader = reinterpret_cast<RoutineUDPHeader*>(pRawPtr);
						const auto pPayload = &pRawPtr[sizeof(RoutineUDPHeader)];

						auto& udpBuffer = foundIP[port];

						if (!udpBuffer)
						{
							udpBuffer = std::make_shared<std::vector<uint8_t>>();
						}

						udpBuffer->resize(static_cast<size_t>(pHeader->FragmentOffset) + pHeader->PacketSize);
						memcpy(&udpBuffer->data()[pHeader->FragmentOffset], pPayload, pHeader->PacketSize);

						if (pHeader->bFragment == 0)
						{
							const auto pMessageHeader = reinterpret_cast<RoutineMessageHeader*>(udpBuffer->data());
							const uint32_t MessageID = pMessageHeader->MessageID;
							udpBuffer->erase(udpBuffer->begin(), udpBuffer->begin() + sizeof(RoutineMessageHeader));

							if (pMessageHeader->Crypto == ROUTINE_CRYPTO_ENCRYPTED)
							{
								if (!m_cryptoKey.empty())
								{
									const auto decBuffer = std::make_shared<std::vector<uint8_t>>();
									const auto pPacketRawPtr = udpBuffer->data();

									const std::vector iv(pPacketRawPtr, pPacketRawPtr + CryptoPP::AES::BLOCKSIZE);

									if (Crypto::DecryptionAES(m_cryptoKey, iv, &udpBuffer->data()[CryptoPP::AES::BLOCKSIZE], udpBuffer->size() - CryptoPP::AES::BLOCKSIZE, *decBuffer))
									{
										m_messageParser.ReceivedMessage(MessageID, ip, port, decBuffer);
									}
								}
							}
							else
							{
								m_messageParser.ReceivedMessage(MessageID, ip, port, udpBuffer);
							}							

							// Switch new Buffer
							udpBuffer = std::make_shared<std::vector<uint8_t>>();
						}
					}
				}
			}
		}
	}
}