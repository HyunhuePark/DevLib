#include "../../include/Graphics/CImage.hpp"

#include "../../include/Graphics/ImageGraphics.hpp"
#include "../../include/Graphics/ImageProcess.hpp"
#include "../../include/Graphics/ImageUtility.hpp"

#ifdef _MSC_VER
#elif defined(__linux__) // Linux
#include <cstring>
#endif

namespace DevLib {
	namespace Graphics {
		;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		CImage::CImage()
		= default;

		CImage::CImage(const CImageObject& img)
			: CImageObject(img)
		{
			
		}

		CImage::CImage(const std::string& imgPath)
		{
			LoadImageFile(imgPath);
		}

		CImage::CImage(int32_t width, int32_t height, ImageType type)
			: CImageObject(width, height, type)
		{
		}

		bool CImage::LoadImageFile(const std::string& imgPath)
		{
			return ImageUtility::LoadImageFile(*this, imgPath);
		}

		bool CImage::LoadImageJpegData(const uint8_t* jpegData, const int32_t size)
		{
			return ImageUtility::LoadJPEGData(*this, jpegData, size);
		}

		bool CImage::LoadImagePngData(const uint8_t* pngData, const int32_t size)
		{
			return ImageUtility::LoadPNGData(*this, pngData, size);
		}

		bool CImage::SaveImageFile(const std::string& imgPath)
		{
			return ImageUtility::SaveImageFile(*this, imgPath);
		}

		void CImage::DrawPoint(int32_t x, int32_t y, CImageColor color)
		{
			ImageGraphics::DrawPoint(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, color);
		}

		void CImage::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor color)
		{
			ImageGraphics::DrawLineBresenham(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x1, y1, x2, y2, color);
		}

		void CImage::DrawCircle(int32_t x, int32_t y, int32_t radius, CImageColor color)
		{
			ImageGraphics::DrawCircleBresenham(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, radius, color);
		}

		void CImage::DrawFillCircle(int32_t x, int32_t y, int32_t radius, CImageColor color)
		{
			ImageGraphics::DrawFillCircleBresenham(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, radius, color);
		}

		void CImage::DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color)
		{
			ImageGraphics::DrawRect(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, width, height, color);
		}

		void CImage::DrawFillRect(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color)
		{
			ImageGraphics::DrawFillRect(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, width, height, color);
		}

		void CImage::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, CImageColor color)
		{
			ImageGraphics::DrawTriangle(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x1, y1, x2, y2, x3, y3, color);
		}

		void CImage::DrawFillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, CImageColor color)
		{
			ImageGraphics::DrawFillTriangle(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x1, y1, x2, y2, x3, y3, color);
		}

		void CImage::DrawEllipse(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color)
		{
			ImageGraphics::DrawEllipse(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, width, height, color);
		}

		void CImage::DrawEllipseEx(int32_t x, int32_t y, int32_t width, int32_t height, double_t rotate_deg,
			CImageColor color, int32_t resolution)
		{
			ImageGraphics::DrawEllipseEx(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, width, height, rotate_deg, color, resolution);
		}

		void CImage::DrawFillEllipse(int32_t x, int32_t y, int32_t width, int32_t height, CImageColor color)
		{
			ImageGraphics::DrawFillEllipse(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, width, height, color);
		}

		void CImage::DrawImage(int32_t x, int32_t y, CImage& img)
		{
			DrawImage(x, y, img.GetWidth(), img.GetHeight(), img);
		}

		void CImage::DrawImage(int32_t x, int32_t y, int32_t width, int32_t height, CImage& img, bool alphaBlend /*= false*/)
		{
			if (IsNull() == false && GetWidth() > x && GetHeight() > y)
			{
				// 색공간 변환
				img.ColorConvert(GetImageType());

				const int32_t srcX = x < 0 ? -x : 0;
				const int32_t srcY = y < 0 ? -y : 0;
				int32_t srcWidth = img.GetWidth() - srcX;
				int32_t srcHeight = img.GetHeight() - srcY;
				const int32_t srcWidthstep = img.GetWidthStep();
				const int32_t srcChannels = img.GetChannels();
				const int32_t dstChannels = GetChannels();

				const int32_t dstX = x < 0 ? 0 : x;
				const int32_t dstY = y < 0 ? 0 : y;

				if (dstX + srcWidth > GetWidth()) srcWidth = GetWidth() - dstX;
				if (dstY + srcHeight > GetHeight()) srcHeight = GetHeight() - dstY;

				uint8_t* pSrc = img.GetImage();
				uint8_t* pDst = GetImage();

				if (srcWidth > 0 && srcHeight > 0)
				{
					if (alphaBlend)
					{
						// Default BGRA
						for (int32_t _y = 0; _y < srcHeight; _y++)
						{
							for (int32_t _x = 0; _x < srcWidth; _x++)
							{
								const auto pDstColor = reinterpret_cast<CImageColor*>(&pDst[(dstY + _y) * GetWidthStep() + (dstX + _x) * GetChannels()]);
								const auto pSrcColor = reinterpret_cast<CImageColor*>(&pSrc[(srcY + _y) * srcWidthstep + (srcX + _x) * srcChannels]);

								double_t aDst = 1.0 - (pSrcColor->a() / 255.0);
								double_t aSrc = 1.0 - aDst;
								CImageColor dstResult = *pDstColor * aDst;
								CImageColor srcResult = *pSrcColor * aSrc;

								pDstColor->b() = dstResult.b() + srcResult.b();
								pDstColor->g() = dstResult.g() + srcResult.g();
								pDstColor->r() = dstResult.r() + srcResult.r();
							}
						}
					}
					else
					{
						for (int32_t _y = 0; _y < srcHeight; _y++)
						{
							memcpy(&pDst[(dstY + _y) * GetWidthStep() + dstX * GetChannels()], &pSrc[(srcY + _y) * srcWidthstep + srcX * srcChannels], static_cast<size_t>(srcWidth) * static_cast<size_t>(dstChannels));
						}
					}
				}
			}
		}

		void CImage::DrawImageBlend(int32_t x, int32_t y, CImage& img, uint32_t mode)
		{
			if (IsNull() == false && GetWidth() > x && GetHeight() > y)
			{
				const int32_t srcX = x < 0 ? -x : 0;
				const int32_t srcY = y < 0 ? -y : 0;
				int32_t srcWidth = img.GetWidth() - srcX;
				int32_t srcHeight = img.GetHeight() - srcY;
				const int32_t srcWidthStep = img.GetWidthStep();
				const int32_t srcChannels = img.GetChannels();

				const int32_t dstX = x < 0 ? 0 : x;
				const int32_t dstY = y < 0 ? 0 : y;

				if (dstX + srcWidth > GetWidth()) srcWidth = GetWidth() - dstX;
				if (dstY + srcHeight > GetHeight()) srcHeight = GetHeight() - dstY;

				uint8_t* pSrc = img.GetImage();
				uint8_t* pDst = GetImage();

				if (img.GetImageType() == GetImageType() && srcWidth > 0 && srcHeight > 0)
				{
					switch (mode)
					{
					case 0:
						for (int32_t _y = 0; _y < srcHeight; _y++)
						{
							for (int32_t _x = 0; _x < srcWidth; _x++)
							{
								const auto pDstColor = reinterpret_cast<CImageColor*>(&pDst[(dstY + _y) * GetWidthStep() + (dstX + _x) * GetChannels()]);
								const auto pSrcColor = reinterpret_cast<CImageColor*>(&pSrc[(srcY + _y) * srcWidthStep + (srcX + _x) * srcChannels]);

								if (pSrcColor->a())
								{
									pDstColor->b() = pSrcColor->b();
									pDstColor->g() = pSrcColor->g();
									pDstColor->r() = pSrcColor->r();
								}
							}
						}
						break;

					case 1 :
						for (int32_t _y = 0; _y < srcHeight; _y++)
						{
							for (int32_t _x = 0; _x < srcWidth; _x++)
							{
								const auto pDstColor = reinterpret_cast<CImageColor*>(&pDst[(dstY + _y) * GetWidthStep() + (dstX + _x) * GetChannels()]);
								const auto pSrcColor = reinterpret_cast<CImageColor*>(&pSrc[(srcY + _y) * srcWidthStep + (srcX + _x) * srcChannels]);

								double_t aDst = 1.0 - (pSrcColor->a() / 255.0);
								double_t aSrc = 1.0 - aDst;
								CImageColor dstResult = *pDstColor * aDst;
								CImageColor srcResult = *pSrcColor * aSrc;

								pDstColor->b() = dstResult.b() + srcResult.b();
								pDstColor->g() = dstResult.g() + srcResult.g();
								pDstColor->r() = dstResult.r() + srcResult.r();
							}
						}
						break;
					default: ;
					}
				}
			}
		}

		void CImage::ColorConvert(ImageType type)
		{
			if (GetImageType() != type)
			{
				CImage temp;
				ColorConvert(type, temp);

				*this = temp;
			}
		}

		void CImage::ColorConvert(ImageType type, CImageObject& dst)
		{
			if (dst.GetImageType() != type || GetWidth() != dst.GetWidth() || GetHeight() != dst.GetHeight())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), type);
			}

			const ImageType TypeSrc = GetImageType();
			const ImageType TypeDst = type;

			if (TypeSrc == ImageType::IMG_GRAY)
			{
				CImageObject temp;
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertGrayToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertGrayToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					ImageProcess::ConvertGrayToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToYCbCr(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_HSV:
					ImageProcess::ConvertGrayToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToHSV(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertGrayToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertGrayToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertGrayToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertGrayToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NV12:
					ImageProcess::ConvertGrayToNV12(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;

				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_BGR555)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					break;
				case ImageType::IMG_RGB:
					break;
				case ImageType::IMG_YCbCR:
					break;
				case ImageType::IMG_HSV:
					break;
				case ImageType::IMG_BGRA:
					break;
				case ImageType::IMG_ABGR:
					break;
				case ImageType::IMG_RGBA:
					break;
				case ImageType::IMG_ARGB:
					break;
				case ImageType::IMG_NONE: 
					break;
				case ImageType::IMG_NV12: 
					break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_BGR565)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_BGR:
					break;
				case ImageType::IMG_RGB:
					break;
				case ImageType::IMG_YCbCR:
					break;
				case ImageType::IMG_HSV:
					break;
				case ImageType::IMG_BGRA:
					break;
				case ImageType::IMG_ABGR:
					break;
				case ImageType::IMG_RGBA:
					break;
				case ImageType::IMG_ARGB:
					break;
				case ImageType::IMG_NONE:
					break;
				case ImageType::IMG_NV12:
					break;
				default:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_BGR)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					ImageProcess::ConvertBGRToGray(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertSwap24(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					ImageProcess::ConvertBGRToYCbCr(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_HSV:
					ImageProcess::ConvertBGRToHSV(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertBGRToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertBGRToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertBGRToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertBGRToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: 
					break;
				case ImageType::IMG_NV12:
					ImageProcess::ConvertBGRToNV12(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_RGB)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					ImageProcess::ConvertRGBToGray(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertSwap24(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_YCbCR:
					ImageProcess::ConvertRGBToYCbCr(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_HSV:
					ImageProcess::ConvertRGBToHSV(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertRGBToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertRGBToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertRGBToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertRGBToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12:
					ImageProcess::ConvertRGBToNV12(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_YCbCR)
			{
				CImageObject temp;
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertYCbCrToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertYCbCrToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_HSV:
					ImageProcess::ConvertHSVToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToYCbCr(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertYCbCrToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertYCbCrToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertYCbCrToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertYCbCrToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: 
					break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_HSV)
			{
				CImageObject temp;
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertHSVToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertHSVToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					ImageProcess::ConvertYCbCrToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToHSV(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_HSV:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertHSVToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertHSVToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertGrayToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertHSVToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: break;
				default:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_BGRA)
			{
				CImageObject temp;
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					ImageProcess::ConvertBGRAToGray(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertBGRAToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertBGRAToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					ImageProcess::ConvertBGRAToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToYCbCr(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_HSV:
					ImageProcess::ConvertBGRAToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					temp = dst;
					ImageProcess::ConvertBGRToHSV(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGRA:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertBGRAToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertBGRAToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertSwap32(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_ABGR)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					break;
				case ImageType::IMG_RGB:
					break;
				case ImageType::IMG_YCbCR:
					break;
				case ImageType::IMG_HSV:
					break;
				case ImageType::IMG_BGRA:
					break;
				case ImageType::IMG_ABGR:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_RGBA:
					break;
				case ImageType::IMG_ARGB:
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_RGBA)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertRGBAToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					break;
				case ImageType::IMG_HSV:
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertRGBAToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertRGBAToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_ARGB:
					ImageProcess::ConvertRGBAToARGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: break;
				default:
					break;
				}
			}
			else if (TypeSrc == ImageType::IMG_ARGB)
			{
				switch (TypeDst)
				{
				case ImageType::IMG_GRAY:
					break;
				case ImageType::IMG_BGR555:
					break;
				case ImageType::IMG_BGR565:
					break;
				case ImageType::IMG_BGR:
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertGrayToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR:
					break;
				case ImageType::IMG_HSV:
					break;
				case ImageType::IMG_BGRA:
					ImageProcess::ConvertGrayToBGRA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ABGR:
					ImageProcess::ConvertARGBToABGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGBA:
					ImageProcess::ConvertARGBToRGBA(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_ARGB:
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_NV12: break;
				default:
					break;
				}
			}
			else if( TypeSrc == ImageType::IMG_NV12 )
			{
				switch (TypeDst) {
				case ImageType::IMG_NONE: break;
				case ImageType::IMG_GRAY:
					ImageProcess::ConvertNV12ToGray(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_BGR555: break;
				case ImageType::IMG_BGR565: break;
				case ImageType::IMG_BGR:
					ImageProcess::ConvertNV12ToBGR(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_RGB:
					ImageProcess::ConvertNV12ToRGB(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
					break;
				case ImageType::IMG_YCbCR: break;
				case ImageType::IMG_HSV: break;
				case ImageType::IMG_BGRA: break;
				case ImageType::IMG_ABGR: break;
				case ImageType::IMG_RGBA: break;
				case ImageType::IMG_ARGB: break;
				case ImageType::IMG_NV12: 
					memcpy(dst.GetImage(), GetImage(), GetSize());
					break;
				default: ;
				}
			}
		}

		void CImage::FlipHorizontal()
		{
			switch (GetImageType())
			{
			case ImageType::IMG_NONE:
				break;
			case ImageType::IMG_GRAY:
				ImageProcess::FlipHorizontal1(GetImage(), GetWidth(), GetHeight());
				break;
			case ImageType::IMG_BGR555:
			case ImageType::IMG_BGR565:
				ImageProcess::FlipHorizontal2(GetImage(), GetWidth(), GetHeight());
				break;
			case ImageType::IMG_BGR:
			case ImageType::IMG_RGB:
			case ImageType::IMG_YCbCR:
			case ImageType::IMG_HSV:
				ImageProcess::FlipHorizontal3(GetImage(), GetWidth(), GetHeight());
				break;
			case ImageType::IMG_BGRA:
			case ImageType::IMG_ABGR:
			case ImageType::IMG_RGBA:
			case ImageType::IMG_ARGB:
				ImageProcess::FlipHorizontal4(GetImage(), GetWidth(), GetHeight());
				break;
			case ImageType::IMG_NV12:
				ImageProcess::FlipHorizontal1(GetImage(), GetWidth(), GetHeight());
				ImageProcess::FlipHorizontal1(GetImage() + GetWidth() * GetHeight(), GetWidth() / 2, GetHeight() / 2);
				break;
			default:;
			}
		}

		void CImage::FlipHorizontal(CImageObject& dst)
		{
			if( dst.GetWidth() != GetWidth() || dst.GetHeight() != GetHeight() || dst.GetImageType() != GetImageType() )
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			switch (GetImageType())
			{
			case ImageType::IMG_NONE:
				break;
			case ImageType::IMG_GRAY:
				ImageProcess::FlipHorizontal1(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
				break;
			case ImageType::IMG_BGR555:
			case ImageType::IMG_BGR565:
				ImageProcess::FlipHorizontal2(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
				break;
			case ImageType::IMG_BGR:
			case ImageType::IMG_RGB:
			case ImageType::IMG_YCbCR:
			case ImageType::IMG_HSV:
				ImageProcess::FlipHorizontal3(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
				break;
			case ImageType::IMG_BGRA:
			case ImageType::IMG_ABGR:
			case ImageType::IMG_RGBA:
			case ImageType::IMG_ARGB:
				ImageProcess::FlipHorizontal4(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
				break;
			case ImageType::IMG_NV12:
				ImageProcess::FlipHorizontal1(GetImage(), GetWidth(), GetHeight(), dst.GetImage());
				ImageProcess::FlipHorizontal1(GetImage() + GetWidth() * GetHeight(), GetWidth() / 2, GetHeight() / 2, dst.GetImage() + GetWidth() * GetHeight());
				break;
			default:;
			}
		}

		void CImage::FlipVertical()
		{
			if( GetImageType() == ImageType::IMG_NV12)
			{
				ImageProcess::FlipVertical(GetImage(), GetWidth(), GetHeight(), 1);
				ImageProcess::FlipVertical(GetImage() + GetWidth() * GetHeight(), GetWidth(), GetHeight(), 1);
			}
			else
			{
				ImageProcess::FlipVertical(GetImage(), GetWidth(), GetHeight(), GetChannels());
			}
		}

		void CImage::FlipVertical(CImageObject& dst)
		{
			if (dst.GetWidth() != GetWidth() || dst.GetHeight() != GetHeight() || dst.GetImageType() != GetImageType())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			if (GetImageType() == ImageType::IMG_NV12)
			{
				ImageProcess::FlipVertical(GetImage(), GetWidth(), GetHeight(), 1, dst.GetImage());
				ImageProcess::FlipVertical(GetImage() + GetWidth() * GetHeight(), GetWidth(), GetHeight(), 1, dst.GetImage() + GetWidth() * GetHeight());
			}
			else
			{
				ImageProcess::FlipVertical(GetImage(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage());
			}
		}

		void CImage::Split(uint8_t* ch1, uint8_t* ch2)
		{
			ImageProcess::SplitChannels(GetImage(), GetWidth(), GetHeight(), ch1, ch2);
		}

		void CImage::Split(uint8_t* ch1, uint8_t* ch2, uint8_t* ch3)
		{
			ImageProcess::SplitChannels(GetImage(), GetWidth(), GetHeight(), ch1, ch2, ch3);
		}

		void CImage::Split(CImageObject& ch1, CImageObject& ch2)
		{
			if (GetChannels() != 2) return;

			if (ch1.IsNull()) ch1.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			else if (GetSize() / 2 != ch1.GetSize())
			{
				ch1.Destroy();
				ch1.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			}

			if (ch2.IsNull()) ch2.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			else if (GetSize() / 2 != ch2.GetSize())
			{
				ch2.Destroy();
				ch2.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			}

			Split(ch1.GetImage(), ch2.GetImage());
		}
				
		void CImage::Split(CImageObject& ch1, CImageObject& ch2, CImageObject& ch3)
		{
			if (GetChannels() != 3) return;

			if (ch1.IsNull()) ch1.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			else if (GetSize() / 3 != ch1.GetSize())
			{
				ch1.Destroy();
				ch1.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			}

			if (ch2.IsNull()) ch2.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			else if (GetSize() / 3 != ch2.GetSize())
			{
				ch2.Destroy();
				ch2.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			}

			if (ch3.IsNull()) ch3.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			else if (GetSize() / 3 != ch3.GetSize())
			{
				ch3.Destroy();
				ch3.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
			}

			Split(ch1.GetImage(), ch2.GetImage(), ch3.GetImage());
		}

		void CImage::Merge(const uint8_t* ch1, const uint8_t* ch2)
		{
			ImageProcess::MergeChannels(GetImage(), GetWidth(), GetHeight(), ch1, ch2);
		}
				
		void CImage::Merge(const uint8_t* ch1, const uint8_t* ch2, const uint8_t* ch3)
		{
			ImageProcess::MergeChannels(GetImage(), GetWidth(), GetHeight(), ch1, ch2, ch3);
		}

		void CImage::Merge(CImageObject& ch1, CImageObject& ch2)
		{
			ImageProcess::MergeChannels(GetImage(), GetWidth(), GetHeight(), ch1.GetImage(), ch2.GetImage());
		}

		void CImage::Merge(CImageObject& ch1, CImageObject& ch2, CImageObject& ch3)
		{
			ImageProcess::MergeChannels(GetImage(), GetWidth(), GetHeight(), ch1.GetImage(), ch2.GetImage(), ch3.GetImage());
		}

		void CImage::HistogramEqualizer()
		{
			if (GetImageType() == ImageType::IMG_GRAY)
			{
				float_t histo[256] = { 0.0, };
				ImageProcess::Histogram(GetImage(), GetWidth(), GetHeight(), histo);
				ImageProcess::HistoEqualizer(GetImage(), GetWidth(), GetHeight(), histo, GetImage());
			}
			else if (GetImageType() == ImageType::IMG_BGR)
			{
				CImageObject img1, img2, img3;
				Split(img1, img2, img3);

				float_t histogram1[256] = { 0.0, };
				ImageProcess::Histogram(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), histogram1);
				ImageProcess::HistoEqualizer(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), histogram1, img1.GetImage());

				float_t histogram2[256] = { 0.0, };
				ImageProcess::Histogram(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), histogram2);
				ImageProcess::HistoEqualizer(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), histogram2, img2.GetImage());

				float_t histogram3[256] = { 0.0, };
				ImageProcess::Histogram(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), histogram3);
				ImageProcess::HistoEqualizer(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), histogram3, img3.GetImage());

				Merge(img1, img2, img3);
			}
		}

		void CImage::HistogramEqualizer(CImage& dst)
		{
			if (dst.GetSize() != GetSize())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			if (GetImageType() == ImageType::IMG_GRAY)
			{
				float_t histo[256] = { 0.0, };
				ImageProcess::Histogram(GetImage(), GetWidth(), GetHeight(), histo);
				ImageProcess::HistoEqualizer(GetImage(), GetWidth(), GetHeight(), histo, dst.GetImage());
			}
			else if (GetImageType() == ImageType::IMG_BGR)
			{
				CImageObject img1, img2, img3;
				Split(img1, img2, img3);

				float_t histogram1[256] = { 0.0, };
				ImageProcess::Histogram(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), histogram1);
				ImageProcess::HistoEqualizer(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), histogram1, dst.GetImage());

				float_t histogram2[256] = { 0.0, };
				ImageProcess::Histogram(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), histogram2);
				ImageProcess::HistoEqualizer(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), histogram2, dst.GetImage());

				float_t histogram3[256] = { 0.0, };
				ImageProcess::Histogram(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), histogram3);
				ImageProcess::HistoEqualizer(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), histogram3, dst.GetImage());

				Merge(img1, img2, img3);
			}
		}

		void CImage::GaussianBlur(double_t sigma /*= 3.0f*/)
		{
			if (GetImageType() == ImageType::IMG_GRAY)
			{
				ImageProcess::Gaussian(GetImage(), GetWidth(), GetHeight(), GetImage(), static_cast<float_t>(sigma));
			}
			else if (GetImageType() == ImageType::IMG_BGR)
			{
				CImageObject img1, img2, img3;
				Split(img1, img2, img3);

				ImageProcess::Gaussian(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), img1.GetImage(), static_cast<float_t>(sigma));
				ImageProcess::Gaussian(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), img2.GetImage(), static_cast<float_t>(sigma));
				ImageProcess::Gaussian(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), img3.GetImage(), static_cast<float_t>(sigma));

				Merge(img1, img2, img3);
			}
		}

		void CImage::GaussianBlur(CImage& dst, double_t sigma /*= 3.0f*/)
		{
			if (dst.GetSize() != GetSize())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			if (GetImageType() == ImageType::IMG_GRAY)
			{
				ImageProcess::Gaussian(GetImage(), GetWidth(), GetHeight(), dst.GetImage(), static_cast<float_t>(sigma));
			}
			else if (GetImageType() == ImageType::IMG_BGR)
			{
				CImageObject img1, img2, img3;
				Split(img1, img2, img3);

				ImageProcess::Gaussian(img1.GetImage(), img1.GetWidth(), img1.GetHeight(), img1.GetImage(), static_cast<float_t>(sigma));
				ImageProcess::Gaussian(img2.GetImage(), img2.GetWidth(), img2.GetHeight(), img2.GetImage(), static_cast<float_t>(sigma));
				ImageProcess::Gaussian(img3.GetImage(), img3.GetWidth(), img3.GetHeight(), img3.GetImage(), static_cast<float_t>(sigma));

				dst.Merge(img1, img2, img3);
			}
		}

		void CImage::Translate(int32_t x, int32_t y)
		{
			CImage temp = *this;
			Clear();
			ImageProcess::Translate(temp.GetImage(), GetWidth(), GetHeight(), GetChannels(), GetImage(), x, y);
		}

		void CImage::Translate(int32_t x, int32_t y, CImage& dst)
		{
			dst.Destroy();
			dst.Create(GetWidth(), GetHeight(), GetImageType());

			ImageProcess::Translate(GetImage(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage(), x, y);
		}

		void CImage::Rotate(float_t angle)
		{
			CImage temp = *this;

			int32_t nwidth = 0, nheight = 0;
			ImageProcess::CalcRotateResultSize(GetWidth(), GetHeight(), angle, &nwidth, &nheight);
			Destroy();
			Create(nwidth, nheight, GetImageType());

			ImageProcess::Rotate(temp.GetImage(), temp.GetWidth(), temp.GetHeight(), GetChannels(), GetImage(), angle);
		}

		void CImage::Rotate(float_t angle, CImage& dst)
		{
			int32_t nwidth = 0, nheight = 0;
			ImageProcess::CalcRotateResultSize(GetWidth(), GetHeight(), angle, &nwidth, &nheight);
			dst.Destroy();
			dst.Create(nwidth, nheight, GetImageType());

			ImageProcess::Rotate(GetImage(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage(), angle);
		}

		void CImage::Resize(int32_t width, int32_t height, ResizeType type/*= RESIZE_NEAREST*/)
		{
			CImage temp;
			Resize(width, height, temp, type);
			*this = temp;
		}

		void CImage::Resize(int32_t width, int32_t height, CImage& dst, ResizeType type /*= RESIZE_NEAREST*/) const
		{
			dst.Destroy();
			dst.Create(width, height, GetImageType());

			switch (type)
			{
			case ResizeType::RESIZE_NEAREST:
				ImageProcess::ResizeNearest(GetImageConst(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage(), width, height);
				break;
			case ResizeType::RESIZE_BILINEAR:
				ImageProcess::ResizeBilinear(GetImageConst(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage(), width, height);
				break;
			case ResizeType::RESIZE_CUBIC:
				ImageProcess::ResizeCubic(GetImageConst(), GetWidth(), GetHeight(), GetChannels(), dst.GetImage(), width, height);
				break;
			}
		}

		void CImage::Scale(float_t scale, ResizeType type /*= RESIZE_NEAREST*/)
		{
			CImage temp;
			Scale(scale, temp, type);
			*this = temp;
		}

		void CImage::Scale(float_t scale, CImage& dst, ResizeType type /*= RESIZE_NEAREST*/) const
		{
			const auto width = static_cast<int32_t>(GetWidth() * scale);
			const auto height = static_cast<int32_t>(GetHeight() * scale);

			Resize(width, height, dst, type);
		}

		void CImage::Extract(int32_t x, int32_t y, int32_t width, int32_t height)
		{
			CImage temp;
			Extract(x, y, width, height, temp);
			*this = temp;
		}

		void CImage::Extract(int32_t x, int32_t y, int32_t width, int32_t height, CImage& dst)
		{
			dst.Destroy();
			dst.Create(width, height, GetImageType());

			int32_t extractX = x;
			int32_t extractY = y;
			int32_t extractWidth = width;
			int32_t extractHeight = height;
			int32_t dstX = 0;
			int32_t dstY = 0;

			if( extractX < 0 )
			{
				dstX = abs(x);
				extractX = 0;
				extractWidth = width + x;
			}
			else if(GetWidth() < extractX)
			{
				extractX = GetWidth();
				extractWidth = 0;
			}
			
			if( extractY < 0 )
			{
				dstY = abs(y);
				extractY = 0;
				extractHeight = height + y;
			}
			else if (GetHeight() < extractY)
			{
				extractY = GetHeight();
				extractHeight = 0;
			}

			if(extractX + extractWidth < 0 )
			{
				extractWidth = 0;
			}
			else if (GetWidth() < extractX + extractWidth)
			{
				extractWidth = extractWidth - (extractX + extractWidth - GetWidth());
			}

			if ( extractY + extractHeight < 0 )
			{
				extractHeight = 0;
			}
			else if( GetHeight() < extractY + extractHeight )
			{
				extractHeight = extractHeight - (extractY + extractHeight - GetHeight());
			}

			ImageProcess::Extract(GetImage(), GetChannels(), GetWidthStep(), extractX, extractY, extractWidth, extractHeight, dst.GetImage(), dst.GetWidthStep(), dstX, dstY);
		}

		void CImage::DrawString(int32_t x, int32_t y, const std::string& str, CImageColor color, int32_t fontSize /*= 20*/)
		{
			const size_t len = str.length();

			ImageGraphics::DrawString(GetImage(), GetWidth(), GetHeight(), GetChannels(), GetWidthStep(), x, y, fontSize, str.c_str(), static_cast<int32_t>(len), color);
		}

		void CImage::Dilation(int32_t maskX, int32_t maskY)
		{
			CImage temp;
			Dilation(maskX, maskY, temp);
			*this = temp;
		}

		void CImage::Dilation(int32_t maskX, int32_t maskY, CImage& dst)
		{
			if (dst.GetWidth() != GetWidth() || dst.GetHeight() != GetHeight() || dst.GetChannels() != GetChannels())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			dst.Clear();

			CImage sb, sg, sr;
			CImage db, dg, dr;

			switch (GetChannels())
			{
			case 1:
				ImageProcess::Dilation(GetImage(), GetWidth(), GetHeight(), dst.GetImage(), maskX, maskY);
				break;
			case 3:
				Split(sb, sg, sr);
				db.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				dg.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				dr.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				ImageProcess::Dilation(sb.GetImage(), GetWidth(), GetHeight(), db.GetImage(), maskX, maskY);
				ImageProcess::Dilation(sg.GetImage(), GetWidth(), GetHeight(), dg.GetImage(), maskX, maskY);
				ImageProcess::Dilation(sr.GetImage(), GetWidth(), GetHeight(), dr.GetImage(), maskX, maskY);
				dst.Merge(db, dg, dr);
				break;
			default:;
			}
		}

		void CImage::Erosion(int32_t maskX, int32_t maskY)
		{
			CImage temp;
			Erosion(maskX, maskY, temp);
			*this = temp;
		}

		void CImage::Erosion(int32_t maskX, int32_t maskY, CImage& dst)
		{
			if (dst.GetWidth() != GetWidth() || dst.GetHeight() != GetHeight() || dst.GetChannels() != GetChannels())
			{
				dst.Destroy();
				dst.Create(GetWidth(), GetHeight(), GetImageType());
			}

			CImage sb, sg, sr;
			CImage db, dg, dr;

			switch (GetChannels())
			{
			case 1:
				ImageProcess::Erosion(GetImage(), GetWidth(), GetHeight(), dst.GetImage(), maskX, maskY);
				break;
			case 3:
				Split(sb, sg, sr);
				db.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				dg.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				dr.Create(GetWidth(), GetHeight(), ImageType::IMG_GRAY);
				ImageProcess::Erosion(sb.GetImage(), GetWidth(), GetHeight(), db.GetImage(), maskX, maskY);
				ImageProcess::Erosion(sg.GetImage(), GetWidth(), GetHeight(), dg.GetImage(), maskX, maskY);
				ImageProcess::Erosion(sr.GetImage(), GetWidth(), GetHeight(), dr.GetImage(), maskX, maskY);
				dst.Merge(db, dg, dr);
				break;
			default:;
			}
		}

		inline void CImage::PseudoFromData(const uint8_t* src, int32_t width, int32_t height)
		{
			if (GetWidth() != width || GetHeight() != height || GetChannels() != 3)
			{
				Destroy();
				Create(width, height);
			}

			int32_t index = 0;
			for (int32_t y = 0; y < height; y++)
			{
				for (int32_t x = 0; x < width; x++)
				{
					DrawPoint(x, y, CImageColor::GetLUTColor(src[index++], 0xFF));
				}
			}
		}

		inline void CImage::PseudoFromData(const uint16_t* src, int32_t width, int32_t height, uint16_t max/*=0xFFFF*/)
		{
			if (GetWidth() != width || GetHeight() != height || GetChannels() != 3)
			{
				Destroy();
				Create(width, height);
			}

			int32_t index = 0;
			for (int32_t y = 0; y < height; y++)
			{
				for (int32_t x = 0; x < width; x++)
				{
					DrawPoint(x, y, CImageColor::GetLUTColor(src[index++], max));
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	}
}
