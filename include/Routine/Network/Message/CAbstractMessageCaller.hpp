#pragma once

#include "../../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		class CAbstractMessageCaller
		{
		public:
			virtual ~CAbstractMessageCaller() = default;
			virtual void OnMessageCall(const string_t& ip, uint16_t port, uint8_t* ptr) = 0;
			virtual std::string GetMessageName() = 0;
		};
	}
}