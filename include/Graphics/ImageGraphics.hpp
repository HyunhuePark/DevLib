#pragma once
#include "CImageColor.hpp"

namespace DevLib {
	namespace Graphics {
		namespace ImageGraphics {

			void DrawPoint(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, CImageColor& color);

			void DrawLineDDA(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor& color);

			void DrawLineBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor& color);

			inline void DrawHorizenLine(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& length, CImageColor& color);

			void DrawCircleBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& radius, CImageColor& color);

			void DrawFillCircleBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& radius, CImageColor& color);

			void DrawRect(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& rectWidth, const int32_t& rectHeight, CImageColor& color);

			void DrawFillRect(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& rectWidth, const int32_t& rectHeight, CImageColor& color);
			void PloatCircle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& x_coor, const int32_t& y_coor, CImageColor& color);

			void FillPloatCircle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& x_coor, int32_t y_coor, CImageColor& color);

			void DrawString(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& fontSize, const char* str, uint32_t strCount, CImageColor& color);

			void DrawTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x1, const int32_t& y1, const int32_t& x2, const int32_t& y2, const int32_t& x3, const int32_t& y3, CImageColor& color);

			void DrawFillTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x1, const int32_t& y1, const int32_t& x2, const int32_t& y2, const int32_t& x3, const int32_t& y3, CImageColor& color);

			void DrawEllipse(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, CImageColor& color);
			void DrawEllipseEx(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, const double_t& rotate_deg, CImageColor& color, const int32_t& resolution = 36);
			void DrawFillEllipse(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, CImageColor& color);

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
		}
	}
}