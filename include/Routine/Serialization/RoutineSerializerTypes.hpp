#pragma once

#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{

		// BitField Define
		struct StBitField8_t
		{
			virtual uint8_t& data() = 0;
		};

		struct StBitField16_t
		{
			virtual uint16_t& data() = 0;
		};

		struct StBitField32_t
		{
			virtual uint32_t& data() = 0;
		};

		struct StBitField64_t
		{
			virtual uint64_t& data() = 0;
		};

	}
}