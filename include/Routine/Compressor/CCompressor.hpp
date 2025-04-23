#pragma once

#include <vector>
#include "../../Base/DevLibTypes.hpp"

namespace  DevLib {

	namespace Routine
	{
		class CCompressor
		{
		public:
			CCompressor(void_ptr src, size_t size, uint32_t CompressLevel = 5);
			CCompressor(std::vector<uint8_t>& src, uint32_t CompressLevel = 5);

			bool Compress() const;

			using CompressedData = std::shared_ptr<std::vector<uint8_t>>;

			CompressedData GetData();

		private:
			void_ptr	m_pSource;
			size_t		m_sizeSource;
			uint32_t	m_levelCompress;
			CompressedData m_compressedData;
		};

	}
}
