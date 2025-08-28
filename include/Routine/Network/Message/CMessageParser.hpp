#pragma once

#include <map>
#include <queue>

#include "CMessageCaller.hpp"
#include "../../Process/CWorker.hpp"
#include "../../Serialization/CSerializer.hpp"
#include "../../../Template/CQueueT.hpp"
#include "../../../Base/CScopeLocker.hpp"

namespace DevLib {
	namespace Routine
	{
		class CMessageParser
		{
		public:
			virtual ~CMessageParser() = default;
			bool Create(WorkerType type = WorkerType::Asynchronous);

			template <typename Class, typename MessageType>
			bool RegisterMessage(uint32_t messageID, void (Class::* Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&), Class* object);

			template <typename MessageType>
			bool RegisterMessage(uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&));

			bool ReceivedMessage(uint32_t messageID, const string_t&, uint16_t, const uint8_t* serializedData, size_t serializedDataSize, bool bDirect = false);
			bool ReceivedMessage(uint32_t messageID, const string_t&, uint16_t, const std::shared_ptr<std::vector<uint8_t>>& serializedData, bool bDirect = false);

			template <typename Message>
			bool WriteMessage(uint32_t messageID, Message& message);

			using MessageInfo = std::vector<std::pair<uint32_t, std::string>>;
			MessageInfo GetRegisteredMessage() const;

			EnableReturnCallback(bool, OnWriteSerializedMessage, const uint32_t messageID, const uint8_t* serializedMessage, const size_t serializedMessageSize)

		private:
			CSerializer m_serializer;
			CCriticalSection m_lockWrite;

			using MessageQue = std::tuple<uint32_t, const string_t, uint16_t, std::shared_ptr<std::vector<uint8_t>>>;
			std::map<uint32_t, std::shared_ptr<CAbstractMessageCaller>> messageMap;
			Template::CQueueT<MessageQue> m_queueMessage;
			CWorker m_workMessageProc;
			void OnMessageProc();
		};

		template <typename Class, typename MessageType>
		bool CMessageParser::RegisterMessage(const uint32_t messageID, void(Class::* const Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&), Class* const object)
		{
			bool bRet = false;
			if (messageMap.find(messageID) == messageMap.end())
			{
				messageMap.emplace(messageID, std::make_shared<CMessageCaller<MessageType>>(std::bind(Func, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
				bRet = true;
			}
			return bRet;
		}

		template <typename MessageType>
		bool CMessageParser::RegisterMessage(const uint32_t messageID, void(*Func)(const string_t&, uint16_t, const std::shared_ptr<MessageType>&))
		{
			bool bRet = false;
			if (messageMap.find(messageID) == messageMap.end())
			{
				messageMap.emplace(messageID, std::make_shared<CMessageCaller<MessageType>>(std::bind(Func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
				bRet = true;
			}
			return bRet;
		}

		template <typename Message>
		bool CMessageParser::WriteMessage(uint32_t messageID, Message& message)
		{
			CScopeLocker lock(m_lockWrite);
			m_serializer.clear();
			m_serializer << message;
			return m_callbackOnWriteSerializedMessage(messageID, m_serializer.data(), m_serializer.size());
		}
	}
}