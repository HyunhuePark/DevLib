#pragma once

#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{

		// BitField Define
		struct StBitField8_t
		{
			virtual ~StBitField8_t() = default;
			virtual uint8_t& data() = 0;
		};

		struct StBitField16_t
		{
			virtual ~StBitField16_t() = default;
			virtual uint16_t& data() = 0;
		};

		struct StBitField32_t
		{
			virtual ~StBitField32_t() = default;
			virtual uint32_t& data() = 0;
		};

		struct StBitField64_t
		{
			virtual ~StBitField64_t() = default;
			virtual uint64_t& data() = 0;
		};

	}
}