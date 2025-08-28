#pragma once

#include "../../IO/CTcpClient.hpp"
#include "../Serialization/CSerializer.hpp"
#include "Message/CMessageParser.hpp"
#include "../../Base/CLocker.hpp"

namespace DevLib {
	namespace Routine
	{
		class CRoutineClient
		{
		public:
			CRoutineClient();
			virtual ~CRoutineClient() = default;

			bool CreateClient(const std::string& ip, uint16_t port, const std::vector<uint8_t>& key = std::vector<uint8_t>());
			void CloseClient();

			template <typename MessageType> bool RegisterMessage(uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&));
			template <typename Class, typename MessageType> bool RegisterMessage(uint32_t messageID, void(Class::* Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&), Class* object);

			template <typename MessageType>
			void WriteMessage(uint32_t messageID, MessageType& pData, bool bTcp = true);

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)

			bool SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count);

			IO::CTcpClient* operator->()
			{
				return &m_sockTCP;
			}

			IO::CTcpClient& operator*()
			{
				return m_sockTCP;
			}

		protected:
			void WriteMessageTCP(uint32_t messageID, const uint8_t* pData, size_t size);
			void WriteMessageUDP(uint32_t messageID, const uint8_t* pData, size_t size);

		private:
			IO::CTcpClient		m_sockTCP;
			IO::CSocket			m_sockUDP;
			CMessageParser		m_messageParser;
			CSerializer			m_serializer;
			CCriticalSection	m_lockerWrite;
			std::vector<uint8_t>	m_cryptoKey;
			std::vector<uint8_t>	m_sendBuffer;

			void OnConnectedServer(const std::string& ip, uint16_t port);
			void OnDisConnectedServer(const std::string& ip, uint16_t port);
			void OnReceivedTCP(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size);
			void OnReceivedUDP(IO::CSocket& sock);

			using SharedBuffer = std::shared_ptr<std::vector<uint8_t>>;
			using SocketPort = uint16_t;
			using SocketIP = std::string;
			std::tuple<SocketIP, SocketPort, SharedBuffer> m_infoServer;

			using PacketUDP = std::tuple<SocketIP, SocketPort, SharedBuffer>;
			CLocker<std::vector<PacketUDP>> m_packetsUDP;
			CWorker	m_workProcessQueue;
			void OnProcessQueue();
		};


		template <typename MessageType>
		bool CRoutineClient::RegisterMessage(const uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&))
		{
			return m_messageParser.RegisterMessage(messageID, Func);
		}

		template <typename Class, typename MessageType>
		bool CRoutineClient::RegisterMessage(const uint32_t messageID, void(Class::* const Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&), Class* const object)
		{
			return m_messageParser.RegisterMessage(messageID, Func, object);
		}

		template <typename MessageType>
		void CRoutineClient::WriteMessage(uint32_t messageID, MessageType& pData, bool bTcp)
		{
			CScopeLocker<CCriticalSection> locker(m_lockerWrite);
			m_serializer.clear();

			m_serializer << pData;

			if (bTcp) WriteMessageTCP(messageID, m_serializer.data(), m_serializer.size());
			else WriteMessageUDP(messageID, m_serializer.data(), m_serializer.size());
		}
	}
}