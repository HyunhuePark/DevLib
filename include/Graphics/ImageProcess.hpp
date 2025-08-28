#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Graphics {
		namespace ImageProcess {

			void Convert888To565(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void Convert888To555(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertBGRToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertRGBToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertBGRAToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRAToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRAToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRAToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRAToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBAToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertARGBToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertGrayToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertBGRToYCbCr(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToYCbCr(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertYCbCrToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertYCbCrToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertYCbCrToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertYCbCrToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertYCbCrToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertYCbCrToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertBGRToHSV(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToHSV(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertHSVToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertHSVToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertHSVToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertHSVToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertHSVToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertHSVToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);


			void ConvertNV12ToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertNV12ToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertNV12ToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertGrayToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertBGRToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertRGBToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void ConvertSwap24(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertSwap32(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void ConvertSwap32Alpha(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void FlipHorizontal1(uint8_t* src, int32_t src_width, int32_t src_height);
			void FlipHorizontal1(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void FlipHorizontal2(uint8_t* src, int32_t src_width, int32_t src_height);
			void FlipHorizontal2(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void FlipHorizontal3(uint8_t* src, int32_t src_width, int32_t src_height);
			void FlipHorizontal3(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);
			void FlipHorizontal4(uint8_t* src, int32_t src_width, int32_t src_height);
			void FlipHorizontal4(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst);

			void FlipVertical(uint8_t* src, int32_t src_width, int32_t src_height, int32_t nChannels);
			void FlipVertical(uint8_t* src, int32_t src_width, int32_t src_height, int32_t nChannels, const uint8_t* dst);

			void SplitChannels(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* ch1, uint8_t* ch2);
			void SplitChannels(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* ch1, uint8_t* ch2, uint8_t* ch3);
			void MergeChannels(uint8_t* dst, int32_t width, int32_t height, const uint8_t* ch1, const uint8_t* ch2);
			void MergeChannels(uint8_t* dst, int32_t width, int32_t height, const uint8_t* ch1, const uint8_t* ch2, const uint8_t* ch3);

			void Histogram(const uint8_t* src, int32_t width, int32_t height, uint32_t histo[256]);
			void Histogram(const uint8_t* src, int32_t width, int32_t height, float_t histo[256]);
			void HistoEqualizer(const uint8_t* src, int32_t width, int32_t height, float_t histo[256], uint8_t* dst);

			void Gaussian(const uint8_t* src, int32_t width, int32_t heigt, uint8_t* dst, float_t sigma = 1.0);

			void Translate(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t mx, int32_t my);

			void CalcRotateResultSize(int32_t width, int32_t height, float_t angle, int32_t* nwidth, int32_t* nheight);
			void Rotate(uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, float_t angle);

			void ResizeNearest(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t dstWidth, int32_t dstHeight);
			void ResizeBilinear(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t dstWidth, int32_t dstHeight);
			void ResizeCubic(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t dstWidth, int32_t dstHeight);

			void Extract(const uint8_t* src, int32_t nChannels, int32_t srcStepLine, int32_t extractX, int32_t extractY, int32_t extractWidth, int32_t extractHeight, uint8_t* dst, int32_t dstStepLine, int32_t dstX, int32_t dstY);

			void Dilation(const uint8_t* src, int32_t width, int32_t height, uint8_t* dst, int32_t mask_x, int32_t mask_y);
			void Erosion(const uint8_t* src, int32_t width, int32_t height, uint8_t* dst, int32_t mask_x, int32_t mask_y);

		}
	}
}

