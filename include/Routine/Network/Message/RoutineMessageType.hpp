#pragma once

#include "../../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		constexpr uint16_t UDP_MAX_PACKET_SIZE = 65000;
		constexpr uint8_t ROUTINE_CRYPTO_UNENCRYPTED = 0;
		constexpr uint8_t ROUTINE_CRYPTO_ENCRYPTED = 1;

		struct RoutineMessageHeader
		{
			uint32_t MessageID{};
			uint32_t MessageLength{};
			uint8_t Crypto{ ROUTINE_CRYPTO_UNENCRYPTED };
		};

		struct RoutineUDPHeader
		{
			uint8_t  bFragment{};
			uint8_t  Reserved{};
			uint16_t PacketSize{};
			uint32_t FragmentOffset{};
		};
	}
}