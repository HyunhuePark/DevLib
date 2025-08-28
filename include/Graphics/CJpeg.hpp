#pragma once

#include "CImageObject.hpp"

namespace DevLib
{
	namespace Graphics
	{
		enum class JPEGSubSampling
		{
			SUBSAMP_420,
			SUBSAMP_422,
			SUBSAMP_444
		};

		class CJpeg
		{
		public:
			CJpeg();
			CJpeg(const CJpeg& src);
			CJpeg(const uint8_t* jpegData, int32_t size);
			CJpeg(const std::string &imgPath);
			CJpeg(DevLib::Graphics::CImageObject& img, uint32_t quality = 90);
			~CJpeg();

			bool LoadImageFile(const std::string& imgPath);
			bool LoadImageMem(const uint8_t* jpegData, int32_t size);
			bool SaveImageFile(const std::string& imgPath) const;

			bool FromImage(CImageObject& img, uint32_t quality = 90, JPEGSubSampling samp = JPEGSubSampling::SUBSAMP_422);
			bool ToImage(CImageObject& img) const;

			int32_t GetWidth() const;
			int32_t GetHeight() const;
			int32_t GetChannels() const;
			uint32_t GetSize() const;

			std::vector<uint8_t>& JpegData();

			// operator
			CJpeg& operator=(const CJpeg& src);
		private:
			int32_t m_width{};
			int32_t m_height{};
			int32_t m_channels{};
			std::vector<uint8_t> m_jpeg;
		};
	}
}