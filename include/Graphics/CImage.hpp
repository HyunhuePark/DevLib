#pragma once

#include "CImageObject.hpp"
#include "CImageColor.hpp"

namespace DevLib
{
	namespace Graphics
	{
		class CImage : public CImageObject
		{
		public:
			CImage();
			CImage(const CImageObject& img);
			CImage(const std::string& imgPath);
			CImage(int32_t width, int32_t height, ImageType type = ImageType::IMG_BGR);

			bool LoadImageFile(const std::string& imgPath);
			bool LoadImageJpegData(const uint8_t* jpegData, const int32_t size);
			bool LoadImagePngData(const uint8_t* pngData, const int32_t size);
			bool SaveImageFile(const std::string& imgPath);

			void DrawPoint(int32_t x, int32_t y, CImageColor color);
			void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor color);
			void DrawCircle(int32_t x, int32_t y, int32_t radius, CImageColor color);
			void DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color);
			void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, CImageColor color);
			void DrawEllipse(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color);
			void DrawEllipseEx(int32_t x, int32_t y, int32_t width, int32_t height, double_t rotate_deg, CImageColor color, int32_t resolution = 36);

			void DrawFillCircle(int32_t x, int32_t y, int32_t radius, CImageColor color);
			void DrawFillRect(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color);
			void DrawFillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, CImageColor color);
			void DrawFillEllipse(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color);

			void DrawImage(int32_t x, int32_t y, CImage& img);			
			void DrawImage(int32_t x, int32_t y, int32_t width, int32_t height, CImage& img, bool alphaBlend = false);
			void DrawImageBlend(int32_t x, int32_t y, CImage& img, uint32_t mode = 0);

			void ColorConvert(ImageType type);
			void ColorConvert(ImageType type, CImageObject& dst);

			void FlipHorizontal();
			void FlipHorizontal(CImageObject& dst);
			void FlipVertical();
			void FlipVertical(CImageObject& dst);
			
			void Split(uint8_t* ch1, uint8_t* ch2);
			void Split(uint8_t* ch1, uint8_t* ch2, uint8_t* ch3);
			void Split(CImageObject& ch1, CImageObject& ch2);
			void Split(CImageObject& ch1, CImageObject& ch2, CImageObject& ch3);

			void Merge(const uint8_t* ch1, const uint8_t* ch2);
			void Merge(const uint8_t* ch1, const uint8_t* ch2, const uint8_t* ch3);
			void Merge(CImageObject& ch1, CImageObject& ch2);
			void Merge(CImageObject& ch1, CImageObject& ch2, CImageObject& ch3);

			void HistogramEqualizer();
			void HistogramEqualizer(CImage& dst);

			void GaussianBlur(double_t sigma = 3.0);
			void GaussianBlur(CImage& dst, double_t sigma = 3.0);

			void Translate(int32_t x, int32_t y);
			void Translate(int32_t x, int32_t y, CImage& dst);

			void Rotate(float_t angle);
			void Rotate(float_t angle, CImage& dst);

			/**
			이미지 크기 변환 알고리즘 종류
			*/

			enum class ResizeType
			{
				RESIZE_NEAREST,		// 최근방 이웃 보간법
				RESIZE_BILINEAR,	// 양선형 보간법
				RESIZE_CUBIC		// 3차 회전 보간법
			};
			void Resize(int32_t width, int32_t height, ResizeType type = ResizeType::RESIZE_BILINEAR);
			void Resize(int32_t width, int32_t height, CImage& dst, ResizeType type = ResizeType::RESIZE_BILINEAR) const;
			void Scale(float_t scale, ResizeType type = ResizeType::RESIZE_BILINEAR);
			void Scale(float_t scale, CImage& dst, ResizeType type = ResizeType::RESIZE_BILINEAR) const;
			void Extract(int32_t x, int32_t y, int32_t width, int32_t height);

			void Extract(int32_t x, int32_t y, int32_t width, int32_t height, CImage& dst);
			void DrawString(int32_t x, int32_t y, const std::string& str, CImageColor color, int32_t fontSize = 20);

			void Dilation(int32_t maskX, int32_t maskY);
			void Dilation(int32_t maskX, int32_t maskY, CImage& dst);

			void Erosion(int32_t maskX, int32_t maskY);
			void Erosion(int32_t maskX, int32_t maskY, CImage& dst);

			void PseudoFromData(const uint8_t* src, int32_t width, int32_t height);
			void PseudoFromData(const uint16_t* src, int32_t width, int32_t height, uint16_t max = 65535);


		};
	}
}