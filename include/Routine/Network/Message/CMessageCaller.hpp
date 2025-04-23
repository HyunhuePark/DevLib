#pragma once

#include "CAbstractMessageCaller.hpp"
#include "../../Serialization/CDeSerializer.hpp"
#include <functional>

namespace DevLib {
	namespace Routine
	{
		template < typename MessageType >
		class CMessageCaller : public CAbstractMessageCaller
		{
		public:
			CMessageCaller(std::function<void(const string_t&, uint16_t, const std::shared_ptr<MessageType>&)> Func);

			std::string GetMessageName() override;

		private:
			std::function<void(const string_t&, uint16_t, const std::shared_ptr<MessageType>&)> m_messageCallback;
			void OnMessageCall(const string_t& ip, uint16_t port, uint8_t* ptr) override;
		};

		template <typename MessageType>
		CMessageCaller<MessageType>::CMessageCaller(std::function<void(const string_t&, uint16_t, const std::shared_ptr<MessageType>&)> Func) : m_messageCallback(std::move(Func))
		{
			// static_assert(IsSharedPtr<MessageType>::value, "MessageType is only shared_ptr");
		}

		template <typename MessageType>
		std::string CMessageCaller<MessageType>::GetMessageName()
		{
			return typeid(MessageType).name();
		}

		template <typename MessageType>
		void CMessageCaller<MessageType>::OnMessageCall(const string_t& ip, uint16_t port, uint8_t* ptr)
		{
			auto sharedData = std::make_shared<MessageType>();
			CDeSerializer deSerializer(ptr);
			deSerializer >> *sharedData;

			m_messageCallback(ip, port, sharedData);
		}
	}
}