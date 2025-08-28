#include "../../../include/Routine/Compressor/CCompressor.hpp"
#include <zlib.h>
#include "../../../include/IO/CFile.hpp"

namespace DevLib {

	namespace Routine {

		CCompressor::CCompressor(void_ptr src, size_t size, uint32_t CompressLevel)
			: m_pSource(src), m_sizeSource(size), m_levelCompress(CompressLevel)
		{
			m_compressedData = std::make_shared<std::vector<uint8_t>>(static_cast<size_t>(static_cast<double_t>(size) * 1.001 + 12));
		}

		CCompressor::CCompressor(std::vector<uint8_t>& src, uint32_t CompressLevel)
			: m_pSource(src.data()), m_sizeSource(src.size()), m_levelCompress(CompressLevel)
		{
			m_compressedData = std::make_shared<std::vector<uint8_t>>(static_cast<size_t>(static_cast<double_t>(src.size()) * 1.001 + 12));
		}

		bool CCompressor::Compress() const
		{
			bool bRet = false;

			auto compressedSize = static_cast<uLongf>(m_compressedData->size());
			const int ret = compress2(m_compressedData->data(), &compressedSize, static_cast<Bytef*>(m_pSource), static_cast<uLong>(m_sizeSource), static_cast<int>(m_levelCompress));

			if (ret == Z_OK)
			{
				m_compressedData->resize(compressedSize);
				bRet = true;
			}
			else
			{
				m_compressedData->resize(0);
			}

			return bRet;
		}

		CCompressor::CompressedData CCompressor::GetData()
		{
			return m_compressedData;
		}

	}
}
