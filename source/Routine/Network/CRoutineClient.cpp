#include "../../../include/Routine/Network/CRoutineClient.hpp"
#include "../../../include/Routine/Network/Message/RoutineMessageType.hpp"

#include <cryptopp/aes.h>
#include "../../../include/Routine/Crypto/CryptoUtility.hpp"

#include "../../../include/IO/DevLibSocketUtility.hpp"

namespace DevLib {
	namespace Routine
	{
		CRoutineClient::CRoutineClient()
		{
			m_sockTCP.RegisterCallbackConnection(&CRoutineClient::OnConnectedServer, this);
			m_sockTCP.RegisterCallbackDisConnection(&CRoutineClient::OnDisConnectedServer, this);
			m_sockTCP.RegisterCallbackReceiveData(&CRoutineClient::OnReceivedTCP, this);

			RegisterCallbackConnection(&CRoutineClient::OnConnection, this);
			RegisterCallbackDisConnection(&CRoutineClient::OnDisConnection, this);

			m_sockTCP.SetBufferSize(sizeof(RoutineMessageHeader));

			m_sockUDP.RegisterCallbackReceive(&CRoutineClient::OnReceivedUDP, this);

			m_workProcessQueue.CreateAsync("CRoutineClient::ProcessQueue", &CRoutineClient::OnProcessQueue, this);
		}

		bool CRoutineClient::CreateClient(const std::string& ip, uint16_t port, const std::vector<uint8_t>& key)
		{
			m_cryptoKey = key;

			m_messageParser.Create();

			return m_sockTCP.CreateTcpClient(ip, port);
		}

		void CRoutineClient::CloseClient()
		{
			m_sockUDP.Destroy();
			m_sockTCP.Destroy();
		}

		void CRoutineClient::WriteMessageTCP(uint32_t messageID, const uint8_t* pData, size_t size)
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
			if (m_sockTCP->WaitForWritable())
			{
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
			}

			m_sendBuffer.clear();
		}

		void CRoutineClient::WriteMessageUDP(uint32_t messageID, const uint8_t* pData, size_t size)
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


			std::vector<uint8_t> udpPacket(UDP_MAX_PACKET_SIZE + sizeof(RoutineUDPHeader));
			const size_t totalSize = (sizeof(RoutineMessageHeader) + m_sendBuffer.size());
			size_t nCount = totalSize / UDP_MAX_PACKET_SIZE;	// Total Size = Routine header + RawData

			auto* udpHeader = reinterpret_cast<RoutineUDPHeader*>(udpPacket.data());
			auto* pUdpBuffer = &udpPacket[sizeof(RoutineUDPHeader)];

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

				m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, std::get<0>(m_infoServer), std::get<1>(m_infoServer));

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
						m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, std::get<0>(m_infoServer), std::get<1>(m_infoServer));
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
				udpHeader->PacketSize = static_cast<uint16_t>(sizeof(RoutineMessageHeader) + m_sendBuffer.size());
				memcpy(&pUdpBuffer[sizeof(RoutineMessageHeader)], m_sendBuffer.data(), m_sendBuffer.size());

				m_sockUDP.SendTo(udpPacket.data(), sizeof(RoutineUDPHeader) + udpHeader->PacketSize, std::get<0>(m_infoServer), std::get<1>(m_infoServer));
			}

			m_sendBuffer.clear();
		}

		bool CRoutineClient::SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count)
		{
			return m_sockTCP.SetKeepAliveTime(second, intervalSecond, count);
		}

		void CRoutineClient::OnConnectedServer(const std::string& ip, uint16_t port)
		{
			std::get<0>(m_infoServer) = ip;
			std::get<1>(m_infoServer) = port;

			m_sockUDP.Create(IO::SOCK_TYPE_UDP);
			m_sockUDP.Bind(IO::SocketUtility::addrToPort(m_sockTCP->GetAddressBind()));
			m_sockUDP.StartWatchDog();

			m_callbackConnection(ip, port);
		}

		void CRoutineClient::OnDisConnectedServer(const std::string& ip, uint16_t port)
		{
			std::get<0>(m_infoServer) = "";
			std::get<1>(m_infoServer) = 0;

			m_sockUDP.Destroy();

			m_callbackDisConnection(ip, port);
		}

		void CRoutineClient::OnReceivedTCP(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size)
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

		void CRoutineClient::OnReceivedUDP(IO::CSocket& sock)
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

		void CRoutineClient::OnProcessQueue()
		{
			m_packetsUDP.Lock();
			const auto vPackets = std::move(m_packetsUDP.get());
			m_packetsUDP.UnLock();

			for (const auto& PacketData : vPackets)
			{
				const auto pRawPtr = std::get<2>(PacketData)->data();
				const auto pHeader = reinterpret_cast<RoutineUDPHeader*>(pRawPtr);
				const auto pPayload = &pRawPtr[sizeof(RoutineUDPHeader)];

				auto& udpBuffer = std::get<2>(m_infoServer);

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
								m_messageParser.ReceivedMessage(MessageID, std::get<0>(PacketData), std::get<1>(PacketData), decBuffer);
							}
						}
					}
					else
					{
						m_messageParser.ReceivedMessage(MessageID, std::get<0>(PacketData), std::get<1>(PacketData), udpBuffer);
					}

					// Switch new Buffer
					udpBuffer = std::make_shared<std::vector<uint8_t>>();
				}
			}
		}
	}
}