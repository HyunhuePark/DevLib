#pragma once

#include "CImageObject.hpp"

namespace DevLib
{
	namespace Graphics
	{
		class CPng
		{
		public:
			CPng();
			CPng(const CPng& src);
			CPng(const uint8_t* pngData, uint32_t size);
			CPng(const std::string& imgPath);
			CPng(const DevLib::Graphics::CImageObject& img);
			~CPng();

			bool LoadImageFile(const std::string& imgPath);
			bool LoadImageMem(const uint8_t* pngData, uint32_t size);
			bool SaveImageFile(const std::string& imgPath) const;

			bool FromImage(const CImageObject& img);
			bool ToImage(CImageObject& img) const;

			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			uint32_t GetChannels() const;
			uint32_t GetSize() const;

			std::vector<uint8_t>& PngData();

			// operator
			CPng& operator=(const CPng& src);
		private:
			uint32_t m_width{};
			uint32_t m_height{};
			uint32_t m_channels{};

			size_t m_readOffset{};
			std::vector<uint8_t> m_png;

			static void PngRead(handle_t pngInfo, uint8_t* pData, size_t length);
			static void PngWrite(handle_t pngInfo, const uint8_t* pData, size_t length);
		};
	}
}