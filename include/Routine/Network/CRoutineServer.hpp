#pragma once

#include <map>

#include "../../IO/CTcpServer.hpp"
#include "../Serialization/CSerializer.hpp"
#include "Message/CMessageParser.hpp"

namespace DevLib {
	namespace Routine
	{
		class CRoutineServer
		{
		public:
			CRoutineServer();

			bool CreateServer(uint16_t port, const std::string& bind = std::string(), bool bBlockMode = true, const std::vector<uint8_t>& key = std::vector<uint8_t>());
			void CloseServer();

			template <typename MessageType> bool RegisterMessage(uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>));
			template <typename Class, typename MessageType> bool RegisterMessage(uint32_t messageID, void(Class::* Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>), Class* object);

			template <typename MessageType>
			void WriteMessage(uint32_t messageID, MessageType& pData, bool bTcp = true);

			template <typename MessageType>
			void WriteMessageTo(const string_t& ip, uint16_t port, uint32_t messageID, MessageType& pData, bool bTcp = true);

			EnableCallback(Connection, const std::string& ip, uint16_t port)
			EnableCallback(DisConnection, const std::string& ip, uint16_t port)

			bool SetKeepAliveTime(uint32_t second, uint32_t intervalSecond, uint32_t count) const;

		protected:
			void WriteMessageTCP(uint32_t messageID, const uint8_t* pData, size_t size);
			void WriteMessageUDP(uint32_t messageID, const uint8_t* pData, size_t size);

			void WriteMessageTCPTo(const string_t& ip, uint16_t port, uint32_t messageID, const uint8_t* pData, size_t size);
			void WriteMessageUDPTo(const string_t& ip, uint16_t port, uint32_t messageID, const uint8_t* pData, size_t size);

		private:
			IO::CTcpServer		m_sockTCP;
			IO::CSocket			m_sockUDP;
			CMessageParser		m_messageParser;
			CSerializer			m_serializer;
			CCriticalSection	m_lockerWrite;
			std::vector<uint8_t>	m_cryptoKey;
			std::vector<uint8_t>	m_sendBuffer;

			void OnConnectedClient(const std::string& ip, uint16_t port);
			void OnDisConnectedClient(const std::string& ip, uint16_t port);
			void OnReceivedTCP(const IO::CSocket& client, const std::string& ip, uint16_t port, void_ptr pData, int32_t size);
			void OnReceivedUDP(IO::CSocket& sock);

			using SharedBuffer = std::shared_ptr<std::vector<uint8_t>>;
			using SocketPort = uint16_t;
			using SocketIP = std::string;
			CLocker<std::map<SocketIP, std::map<SocketPort, SharedBuffer>>> m_infoClient;

			using PacketUDP = std::tuple<SocketIP, SocketPort, SharedBuffer>;
			CLocker<std::vector<PacketUDP>> m_packetsUDP;
			CWorker	m_workProcessQueue;
			void OnProcessQueue();
		};


		template <typename MessageType>
		bool CRoutineServer::RegisterMessage(const uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>))
		{
			return m_messageParser.RegisterMessage(messageID, Func);
		}

		template <typename Class, typename MessageType>
		bool CRoutineServer::RegisterMessage(const uint32_t messageID, void(Class::* const Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>),
			Class* const object)
		{
			return m_messageParser.RegisterMessage(messageID, Func, object);
		}

		template <typename MessageType>
		void CRoutineServer::WriteMessage(uint32_t messageID, MessageType& pData, bool bTcp)
		{
			CScopeLocker<CCriticalSection> locker(m_lockerWrite);
			m_serializer.clear();

			m_serializer << pData;

			if (bTcp) WriteMessageTCP(messageID, m_serializer.data(), m_serializer.size());
			else WriteMessageUDP(messageID, m_serializer.data(), m_serializer.size());
		}

		template <typename MessageType>
		void CRoutineServer::WriteMessageTo(const string_t& ip, const uint16_t port, uint32_t messageID, MessageType& pData, bool bTcp)
		{
			CScopeLocker<CCriticalSection> locker(m_lockerWrite);
			m_serializer.clear();

			m_serializer << pData;

			if (bTcp) WriteMessageTCPTo(ip, port, messageID, m_serializer.data(), m_serializer.size());
			else WriteMessageUDPTo(ip, port, messageID, m_serializer.data(), m_serializer.size());
		}
	}
}