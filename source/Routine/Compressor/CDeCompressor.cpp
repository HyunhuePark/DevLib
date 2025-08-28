#include "../../../include/Routine/Compressor/CDeCompressor.hpp"

#include <zlib.h>

namespace DevLib {
	namespace Routine {

		CDeCompressor::CDeCompressor(void_ptr compressedData, size_t compressedSize)
			: m_pCompressedData(compressedData), m_sizeCompressed(compressedSize)
		{
			m_deCompressedData = std::make_shared<std::vector<uint8_t>>(compressedSize * 2);
		}

		CDeCompressor::CDeCompressor(std::vector<uint8_t>& compressedData)
			: m_pCompressedData(compressedData.data()), m_sizeCompressed(compressedData.size())
		{
			m_deCompressedData = std::make_shared<std::vector<uint8_t>>(compressedData.size() * 2);
		}

		bool CDeCompressor::DeCompress() const
		{
			bool bRet = false;
			auto deCompressedSize = static_cast<uLongf>(m_deCompressedData->size());
			int ret = uncompress(m_deCompressedData->data(), &deCompressedSize, static_cast<Bytef*>(m_pCompressedData), static_cast<uLong>(m_sizeCompressed));

			int nCnt = 10;
			while (ret != Z_OK && nCnt--)
			{
				m_deCompressedData->resize(m_deCompressedData->size() + static_cast<size_t>(static_cast<double_t>(m_deCompressedData->size()) * 2));
				deCompressedSize = static_cast<uLongf>(m_deCompressedData->size());
				ret = uncompress(m_deCompressedData->data(), &deCompressedSize, static_cast<Bytef*>(m_pCompressedData), static_cast<uLong>(m_sizeCompressed));
			}

			if (nCnt > 0)
			{
				bRet = true;
				m_deCompressedData->resize(deCompressedSize);
			}
			else
			{
				m_deCompressedData->resize(0);
			}

			return bRet;
		}

		CDeCompressor::DeCompressedData CDeCompressor::GetData()
		{
			return m_deCompressedData;
		}
	}
}
