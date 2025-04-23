#pragma once

#include "CImageObject.hpp"

namespace DevLib
{
	namespace Graphics
	{
		class CJpeg
		{
		public:
			CJpeg();
			CJpeg(const CJpeg& src);
			CJpeg(const uint8_t* jpegData, const int32_t size);
			CJpeg(const std::string &imgPath);
			CJpeg(DevLib::Graphics::CImageObject& img, uint32_t quality = 90);
			~CJpeg();

			bool LoadImageFile(const std::string& imgPath);
			bool LoadImageMem(const uint8_t* jpegData, const int32_t size);
			bool SaveImageFile(const std::string& imgPath) const;

			bool FromImage(CImageObject& img, uint32_t quality = 90);
			bool ToImage(CImageObject& img) const;

			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			uint32_t GetChannels() const;
			uint32_t GetSize() const;

			std::vector<uint8_t>& JpegData();

			// operator
			CJpeg& operator=(const CJpeg& src);
		private:
			uint32_t m_width{};
			uint32_t m_height{};
			uint32_t m_channels{};
			std::vector<uint8_t> m_jpeg;

			static void JpegInit(handle_t jpegInfo);
			static void JpegTerm(handle_t jpegInfo);
			static uint8_t JpegEmptyMemory(handle_t jpegInfo);
		};
	}
}