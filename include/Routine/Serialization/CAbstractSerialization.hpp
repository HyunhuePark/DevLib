#pragma once

#include "RoutineSerializerTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		class CSerializer;
		class CDeSerializer;

		class CAbstractSerialization
		{
		public:
			CAbstractSerialization(uint32_t _messageID = 0) : messageID(_messageID){}
			virtual ~CAbstractSerialization() = default;
			virtual void Serialization(CSerializer& serializer) = 0;
			virtual void DeSerialization(CDeSerializer& deserializer) = 0;

			template <typename  Type>
			void Swap(Type& value);

			void SetMessageID(uint32_t id)
			{
				messageID = id;
			}

			uint32_t GetMessageID() const
			{
				return messageID;
			}

		private:
			uint32_t messageID{};
		};

		template <typename Type>
		void CAbstractSerialization::Swap(Type& value)
		{
			Type temp = value;
			const auto pSrc = reinterpret_cast<uint8_t*>(&temp);
			const auto pDst = reinterpret_cast<uint8_t*>(&value);

			for( size_t i = 0; i < sizeof(Type); i++)
			{
				pDst[i] = pSrc[sizeof(Type) - 1 - i];
			}
		}
	}
}