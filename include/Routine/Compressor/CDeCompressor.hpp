#pragma once

#include <vector>
#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine {

		class CDeCompressor
		{
		public:
			CDeCompressor(void_ptr compressedData, size_t compressedSize);
			CDeCompressor(std::vector<uint8_t>& compressedData);

			bool DeCompress() const;

			using DeCompressedData = std::shared_ptr<std::vector<uint8_t>>;
			DeCompressedData GetData();
		private:
			void_ptr	m_pCompressedData;
			size_t		m_sizeCompressed;
			DeCompressedData m_deCompressedData;
		};

	}
}
