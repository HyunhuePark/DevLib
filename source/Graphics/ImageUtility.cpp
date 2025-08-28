#include "../../include/Graphics/ImageUtility.hpp"

#include "../../include/Graphics/ImageProcess.hpp"
#include "../../include/Graphics/CImage.hpp"
#include "../../include/IO/CFile.hpp"
#include "../../include/Graphics/CJpeg.hpp"
#include "../../include/Graphics/CPng.hpp"

#include <algorithm>

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux

#include <cstring>

typedef struct {
	uint8_t    rgbBlue;
	uint8_t    rgbGreen;
	uint8_t    rgbRed;
	uint8_t    rgbReserved;
} RGBQUAD;

typedef struct {
	unsigned short		bfType;
	unsigned long		bfSize;
	unsigned short		bfReserved1;
	unsigned short		bfReserved2;
	unsigned long		bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	int32_t	bcSize;
	int16_t bcWidth;
	int16_t bcHeight;
	int16_t bcPlanes;
	int16_t bcBitCount;
} BITMAPCOREHEADER;

typedef struct {
	unsigned long       biSize;
	long				biWidth;
	long				biHeight;
	unsigned short      biPlanes;
	unsigned short      biBitCount;
	unsigned long		biCompression;
	unsigned long		biSizeImage;
	long				biXPelsPerMeter;
	long				biYPelsPerMeter;
	unsigned long       biClrUsed;
	unsigned long       biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO;

typedef struct
{
	long    left;
	long    top;
	long    right;
	long    bottom;
} RECT;

#endif


namespace DevLib
{
	namespace Graphics
	{
		namespace ImageUtility
		{

			int Get4DivVal(int nVal)
			{
				const int32_t UBit = (nVal & 2) >> 1;
				const int32_t LBit = (nVal & 1);
				const int32_t result = ((UBit ^ LBit) << 1) | LBit;

				return result; // ==>  ( nVal + result ) % 4 ==0; 
			}
			 
			void DrawBitmapToDC(void* Target, const CImageObject& image, bool bStretch)
			{
#if defined(_MSC_VER)

				const uint8_t* pImage;
				const int32_t width = image.GetWidth();
				const int32_t height = -image.GetHeight();
				const size_t imgSize = image.GetSize();
				std::vector<uint8_t>imgTmp;

				if (image.IsNull()) return;

				if (image.GetWidth() & 3)
				{
					const int32_t nAdd = Get4DivVal(image.GetWidthStep());

					const uint32_t tmpWidthStep = image.GetWidthStep() + nAdd;
					imgTmp.resize(tmpWidthStep * static_cast<size_t>(image.GetHeight()));

					constexpr uint8_t tmpEmpty[4] = { 0, };

					for (int y = 0; y < image.GetHeight(); y++)
					{
						memcpy(&imgTmp[y * tmpWidthStep], &image.GetImageConst()[y * image.GetWidthStep()], image.GetWidthStep());
						if (nAdd) memcpy(&imgTmp[y * tmpWidthStep + image.GetWidthStep()], tmpEmpty, nAdd);
					}

					pImage = imgTmp.data();
				}
				else
				{
					pImage = image.GetImageConst();
				}

				CImageObject cvt;
				if (image.GetImageType() == ImageType::IMG_RGB)
				{
					cvt.Create(image.GetWidth(), image.GetHeight(), ImageType::IMG_BGR);
					ImageProcess::ConvertSwap24(image.GetImageConst(), image.GetWidth(), image.GetHeight(), cvt.GetImage());
					pImage = cvt.GetImage();
				}
				else if (image.GetImageType() == ImageType::IMG_RGBA)
				{
					cvt.Create(image.GetWidth(), image.GetHeight(), ImageType::IMG_BGRA);
					ImageProcess::ConvertSwap32(image.GetImageConst(), image.GetWidth(), image.GetHeight(), cvt.GetImage());
					pImage = cvt.GetImage();
				}


				BITMAPINFO bmi;
				const BITMAPINFO* pBmi = &bmi;
				memset(&bmi, 0, sizeof(BITMAPINFO));
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biWidth = width;
				bmi.bmiHeader.biHeight = height;
				bmi.bmiHeader.biBitCount = image.GetChannels() * 8;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = static_cast<uint32_t>(imgSize);

				if (image.GetImageType() == ImageType::IMG_GRAY)
				{
					uint8_t bmi_pal[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];

					RGBQUAD pal[256];
					for (int i = 0; i < 256; i++)
						pal[i].rgbBlue = pal[i].rgbGreen = pal[i].rgbRed = i;

					memcpy(bmi_pal, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
					memcpy(bmi_pal + sizeof(BITMAPINFOHEADER), pal, 256 * sizeof(RGBQUAD));

					pBmi = reinterpret_cast<BITMAPINFO*>(bmi_pal);
				}


				const HDC hdc = static_cast<HDC>(Target);
				RECT rect;
				GetClientRect(WindowFromDC(hdc), &rect);

				float xRatio = 1.0;
				float yRatio = 1.0;
				if (bStretch)
				{
					xRatio = static_cast<float>(rect.right) / static_cast<float>(image.GetWidth());
					yRatio = static_cast<float>(rect.bottom) / static_cast<float>(image.GetHeight());
				}

				SetStretchBltMode(hdc, COLORONCOLOR);
				StretchDIBits(hdc,
					0, 0,
					static_cast<int>(image.GetWidth() * xRatio), static_cast<int>(image.GetHeight() * yRatio),
					0,
					0,
					image.GetWidth(),
					image.GetHeight(),
					pImage, pBmi, DIB_RGB_COLORS, SRCCOPY);
#elif defined(__linux__)	// Linux

#endif
			}

			void DrawDCToBitmap(void* Source, CImageObject& image)
			{
#if defined(_MSC_VER)
				// HDC 
				RECT windowRect;
				const HWND hWnd = WindowFromDC(static_cast<HDC>(Source));
				if (GetParent(hWnd)) GetWindowRect(hWnd, &windowRect);
				else GetClientRect(hWnd, &windowRect);


				// Image Size
				int width = windowRect.right - windowRect.left;
				const int32_t height = windowRect.bottom - windowRect.top;

				const int32_t adder = Get4DivVal(width);
				width += adder;

				BITMAPINFO info;
				info.bmiHeader.biSize = sizeof(info.bmiHeader);
				info.bmiHeader.biWidth = width;
				info.bmiHeader.biHeight = -height;
				info.bmiHeader.biPlanes = 1;
				info.bmiHeader.biBitCount = 24;
				info.bmiHeader.biCompression = BI_RGB;
				info.bmiHeader.biSizeImage = 0;
				info.bmiHeader.biXPelsPerMeter = 0;
				info.bmiHeader.biYPelsPerMeter = 0;
				info.bmiHeader.biClrUsed = 0;
				info.bmiHeader.biClrImportant = 0;

				//
				image.Destroy();
				image.Create(width - adder, height, ImageType::IMG_BGR);

				// 
				const HDC memDC = CreateCompatibleDC(static_cast<HDC>(Source));
				uint8_t* buff = nullptr;
				const HBITMAP hBitmap = CreateDIBSection(static_cast<HDC>(Source), &info, DIB_RGB_COLORS, reinterpret_cast<void**>(&buff), nullptr, 0);
				SelectObject(memDC, hBitmap);
				BitBlt(memDC, 0, 0, width, height, static_cast<HDC>(Source), 0, 0, SRCCOPY);

				// CopyImage
				if (adder)
				{
					uint8_t* dst = image.GetImage();
					const int32_t widthStepDst = image.GetWidthStep();
					const int32_t widthStepSrc = (info.bmiHeader.biBitCount / 8) * info.bmiHeader.biWidth;

					for (int y = 0; y < height; y++)
					{
						memcpy(&dst[widthStepDst * y], &buff[widthStepSrc * y], widthStepDst);
					}
				}
				else
				{
					memcpy(image.GetImage(), buff, image.GetSize());
				}

				DeleteDC(memDC);
				DeleteObject(hBitmap);
#elif defined(__linux__) // Linux
#endif
			}

			bool SaveImageFile(CImageObject& image, const std::string& szFileName)
			{
				bool bRet = false;
				std::string filePath = szFileName;
				std::transform(szFileName.begin(), szFileName.end(), filePath.begin(), tolower);
				const size_t extLen = filePath.length() - filePath.rfind('.');

				const std::string ext = filePath.substr(filePath.length() - extLen, extLen);

				if (ext == ".bmp") bRet = SaveBitmapFile(image, szFileName);
				if (ext == ".jpg") bRet = SaveJPEGFile(image, szFileName);
				if (ext == ".jpeg") bRet = SaveJPEGFile(image, szFileName);
				if (ext == ".png") bRet = SavePNGFile(image, szFileName);

				return bRet;
			}

			bool LoadImageFile(CImageObject& image, const std::string& szFileName)
			{
				bool bRet = false;

				if (!szFileName.empty())
				{
					std::string filePath = szFileName;
					std::transform(szFileName.begin(), szFileName.end(), filePath.begin(), tolower);
					const size_t extLen = filePath.length() - filePath.rfind('.');

					const std::string ext = filePath.substr(filePath.length() - extLen, extLen);

					if (ext == ".bmp") bRet = LoadBitmapFile(image, szFileName);
					if (ext == ".jpg") bRet = LoadJPEGFile(image, szFileName);
					if (ext == ".jpeg") bRet = LoadJPEGFile(image, szFileName);
					if (ext == ".png") bRet = LoadPNGFile(image, szFileName);

					if (!image.empty())
					{
						if (image.GetImageType() == ImageType::IMG_RGBA)
						{
							const auto cImage = reinterpret_cast<CImage*>(&image);
							cImage->ColorConvert(ImageType::IMG_BGRA);
						}
						else if (image.GetImageType() == ImageType::IMG_RGB)
						{
							const auto cImage = reinterpret_cast<CImage*>(&image);
							cImage->ColorConvert(ImageType::IMG_BGR);
						}
					}
				}

				return bRet;
			}


			bool LoadBitmapFile(CImageObject& image, const std::string& szFileName)
			{
				BITMAPFILEHEADER bmf;
				uint32_t dwHeaderSize = 0;

				RGBQUAD quadPalette[256];

				IO::CFile file;
				if (file.Open(szFileName, "rb") != true)
				{
					return false;
				}

				int nImageType;
				int offset;
				size_t nSizePalette;
				int width;
				int widthDiff;
				int height;

				if (file.Read(&bmf, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
					return false; //BITMAPFILEHEADER 

				if (file.Read(&dwHeaderSize, sizeof(uint32_t)) != sizeof(uint32_t))
					return false;

				if (dwHeaderSize == sizeof(BITMAPCOREHEADER))
				{
					// OS/2
					BITMAPCOREHEADER bmi;
					if (file.Read(reinterpret_cast<uint8_t*>(&bmi) + sizeof(uint32_t), sizeof(BITMAPCOREHEADER) - sizeof(uint32_t)) != sizeof(BITMAPCOREHEADER) - sizeof(uint32_t))
						return false;

					nImageType = bmi.bcBitCount / 8;
					offset = bmf.bfOffBits;
					nSizePalette = offset - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPCOREHEADER);
					width = bmi.bcWidth;
					widthDiff = Get4DivVal(bmi.bcWidth * nImageType);
					height = bmi.bcHeight > 0 ? bmi.bcHeight : -bmi.bcHeight;
				}
				else if (dwHeaderSize == sizeof(BITMAPINFOHEADER))
				{
					//  Windows version 3 
					BITMAPINFOHEADER bmi;
					if (file.Read((reinterpret_cast<uint8_t*>(&bmi) + sizeof(uint32_t)), sizeof(BITMAPINFOHEADER) - sizeof(uint32_t)) != sizeof(BITMAPINFOHEADER) - sizeof(uint32_t))
						return false;

					nImageType = bmi.biBitCount / 8;
					offset = bmf.bfOffBits;
					nSizePalette = offset - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
					width = bmi.biWidth;
					widthDiff = Get4DivVal(bmi.biWidth * nImageType);
					height = bmi.biHeight > 0 ? bmi.biHeight : -bmi.biHeight;
				}
				else return false;


				if (file.Read(&quadPalette, nSizePalette) != nSizePalette) return false;

				image.Destroy();
				switch (nImageType)
				{
				case 1:
					image.Create(width, height, ImageType::IMG_GRAY);
					break;
				case 2:
					image.Create(width, height, ImageType::IMG_BGR565);
					break;
				case 3:
					image.Create(width, height, ImageType::IMG_BGR);
					break;
				case 4:
					image.Create(width, height, ImageType::IMG_BGRA);
					break;
				default:
					image.Create(width, height, ImageType::IMG_BGR);
				}

				uint8_t* imageData = image.GetImage();

				char Temp[16];
				for (int y = height - 1; y >= 0; y--)
				{
					file.Read(&imageData[y * image.GetWidthStep()], image.GetWidthStep());
					file.Read(&Temp, widthDiff);
				}

				file.Close();

				return true;
			}


			bool SaveBitmapFile(CImageObject& image, const std::string& szFileName)
			{
				const size_t nImageSize = image.GetSize();

				BITMAPFILEHEADER bmf;
				BITMAPINFOHEADER bmi;
				RGBQUAD quadPalette[256];

				bmf.bfType = (static_cast<unsigned short>('M' << 8) | 'B');	//BM
				bmf.bfSize = static_cast<unsigned long>(sizeof(BITMAPFILEHEADER)) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + static_cast<uint32_t>(nImageSize);		// nDibSize 
				bmf.bfReserved1 = bmf.bfReserved2 = 0;	// 
				bmf.bfOffBits = static_cast<unsigned long>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
				if (image.GetImageType() == ImageType::IMG_GRAY) bmf.bfOffBits += sizeof(RGBQUAD) * 256;

				memset(&bmi, 0, sizeof(BITMAPINFOHEADER));
				bmi.biSize = sizeof(BITMAPINFOHEADER);
				bmi.biPlanes = 1;
				bmi.biWidth = image.GetWidth();
				bmi.biHeight = image.GetHeight();
				bmi.biBitCount = image.GetChannels() * 8;
				bmi.biCompression = 0L;
				bmi.biSizeImage = static_cast<uint32_t>(image.GetSize());

				for (int i = 0; i < 256; i++)
					quadPalette[i].rgbBlue = quadPalette[i].rgbGreen = quadPalette[i].rgbRed = i;

				IO::CFile file;
				file.Open(szFileName, "wb");
				if (file.Write(&bmf, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER)) return false;

				if (file.Write(&bmi, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER)) return false;

				if (image.GetImageType() == ImageType::IMG_GRAY) (void)file.Write(&quadPalette, 256 * sizeof(RGBQUAD));

				constexpr uint8_t TmpEmpty[4] = { 0, };
				for (int i = image.GetHeight() - 1; i >= 0; i--)
				{
					file.Write(&image.GetImage()[i * image.GetWidthStep()], image.GetWidthStep());
					file.Write(TmpEmpty, Get4DivVal(image.GetWidthStep())); // 4
				}


				file.Close();
				return true;
			}

			bool LoadJPEGFile(CImageObject& image, const std::string& szFileName)
			{
				bool bRet = false;

				CJpeg jpeg;
				if (jpeg.LoadImageFile(szFileName))
				{
					bRet = jpeg.ToImage(image);
				}

				return bRet;
			}

			bool LoadJPEGData(CImageObject& image, const uint8_t* jpegData, const int32_t size)
			{
				bool bRet = false;

				const CJpeg jpeg(jpegData, size);
				if (jpeg.GetSize() )
				{
					bRet = jpeg.ToImage(image);
				}

				return bRet;
			}

			bool SaveJPEGFile(CImageObject& image, const std::string& szFileName, int32_t q/* = 80*/)
			{
				bool bRet = false;

				CJpeg jpeg;
				if (jpeg.FromImage(image, q))
				{
					bRet = jpeg.SaveImageFile(szFileName);
				}

				return bRet;
			}

			bool LoadPNGFile(CImageObject& image, const std::string& szFileName)
			{
				bool bRet = false;

				CPng png;
				if (png.LoadImageFile(szFileName))
				{
					bRet = png.ToImage(image);
				}

				return bRet;
			}

			bool LoadPNGData(CImageObject& image, const uint8_t* pngData, const int32_t size)
			{
				bool bRet = false;

				const CPng png(pngData, size);
				if (png.GetSize())
				{
					bRet = png.ToImage(image);
				}

				return bRet;
			}

			bool SavePNGFile(const CImageObject& image, const std::string& szFileName)
			{
				bool bRet = false;

				CPng png;
				if (png.FromImage(image))
				{
					bRet = png.SaveImageFile(szFileName);
				}

				return bRet;
			}
		}
	}
}
