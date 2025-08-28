#include "../../../../include/Routine/Network/Message/CMessageParser.hpp"

namespace DevLib {
	namespace Routine
	{
		bool CMessageParser::Create(WorkerType type)
		{
			bool bRet = false;
			if (!m_workMessageProc.IsCreated())
			{
				m_workMessageProc.Create(type, "CMessageParser::OnMessageProc", &CMessageParser::OnMessageProc, this);
				bRet = true;
			}

			return bRet;
		}

		CMessageParser::MessageInfo CMessageParser::GetRegisteredMessage() const
		{
			MessageInfo msgInfo;
			for (const auto& element : messageMap)
			{
				msgInfo.emplace_back(element.first, element.second->GetMessageName());
			}

			return msgInfo;
		}

		bool CMessageParser::ReceivedMessage(uint32_t messageID, const string_t& ip, uint16_t port, const uint8_t* serializedData, size_t serializedDataSize, bool bDirect)
		{
			bool bRet = false;
			if (messageMap.find(messageID) != messageMap.end())
			{
				if (bDirect)
				{
					messageMap[messageID]->OnMessageCall(ip, port, const_cast<uint8_t*>(serializedData));
					bRet = true;
				}
				else
				{
					const std::shared_ptr<std::vector<uint8_t>> temp = std::make_shared<std::vector<uint8_t>>(serializedDataSize);
					temp->assign(serializedData, serializedData + serializedDataSize);
					bRet = ReceivedMessage(messageID, ip, port, temp);
				}
			}

			return bRet;
		}

		bool CMessageParser::ReceivedMessage(uint32_t messageID, const string_t& ip, uint16_t port, const std::shared_ptr<std::vector<uint8_t>>& serializedData, bool bDirect)
		{
			bool bRet = false;
			if (messageMap.find(messageID) != messageMap.end())
			{
				if (bDirect)
				{
					messageMap[messageID]->OnMessageCall(ip, port, serializedData->data());
				}
				else
				{
					m_queueMessage.emplace(messageID, ip, port, serializedData);

					m_workMessageProc.Work();
				}
				bRet = true;
			}

			return bRet;
		}

		void CMessageParser::OnMessageProc()
		{
			m_queueMessage.Lock();
			auto queue = std::move(*m_queueMessage); 
			m_queueMessage.UnLock();

			while (!queue.empty())
			{
				auto data = queue.front();

				messageMap[std::get<0>(data)]->OnMessageCall(std::get<1>(data), std::get<2>(data), std::get<3>(data)->data());
				queue.pop();
			}
		}
	}
}