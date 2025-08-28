#include "../../include/Visualization/CImageViewer.hpp"

#include "../../include/Graphics/CImageObject.hpp"
#include "../../include/Graphics/ImageUtility.hpp"
#include "../../include/Graphics/ImageProcess.hpp"

#include <cstring>

namespace DevLib
{
	namespace Visualization
	{

		CImageViewer::CImageViewer(std::string title)
			: CDevLibWindow(std::move(title))
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
			  ,
			  m_xImage(nullptr)
#endif
		{
		}

		CImageViewer::~CImageViewer()
		{
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
			if (m_xImage)
			{
				XDestroyImage(m_xImage);
				m_xImage = nullptr;
			}
#endif
		}

		void CImageViewer::ShowImage(const std::string &image_path)
		{
			// Image Load
			Graphics::CImageObject image;
			Graphics::ImageUtility::LoadImageFile(image, image_path);

			// ImageDraw
			ShowImage(image);
		}

		void CImageViewer::ShowImage(const int32_t width, const int32_t height, const uint8_t *img, const Graphics::ImageType type)
		{
			// Image Create & Copy
			Graphics::CImageObject image;
			image.Create(width, height, type);
			memcpy(image.GetImage(), img, image.GetSize());

			// ImageDraw
			ShowImage(image);
		}

		void CImageViewer::ShowImage(const Graphics::CImageObject &image)
		{
			// Window Size & Position
			if (!image.IsNull())
				SetResize(image.GetWidth(), image.GetHeight());

			Show();

#if defined(_MSC_VER)
			Graphics::ImageUtility::DrawBitmapToDC(GetDC(), image, false);
#elif defined(__linux__) // Linux

			if (m_image.GetWidth() != image.GetWidth() || m_image.GetHeight() != image.GetHeight())
			{
				m_image.Destroy();
				m_image.Create(image.GetWidth(), image.GetHeight(), DevLib::Graphics::ImageType::IMG_BGRA);

				if (m_xImage)
				{
					XDestroyImage(m_xImage);
					m_xImage = nullptr;
				}

				char_t *pBuff = new char_t[m_image.GetSize()];
				m_xImage = XCreateImage(GetDC().display, GetDC().visualInfo->visual, 24, ZPixmap, 0,
										(char_t *)pBuff, m_image.GetWidth(), m_image.GetHeight(),
										32, 0);
			}

			if (image.GetSize() > 0)
			{
				// Cvt 32Bit Image
				switch (image.GetImageType())
				{
				case DevLib::Graphics::ImageType::IMG_NONE:
					break;
				case DevLib::Graphics::ImageType::IMG_GRAY:
					DevLib::Graphics::ImageProcess::ConvertGrayToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_BGR555:
					break;
				case DevLib::Graphics::ImageType::IMG_BGR565:
					break;
				case DevLib::Graphics::ImageType::IMG_BGR:
					DevLib::Graphics::ImageProcess::ConvertBGRToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_RGB:
					DevLib::Graphics::ImageProcess::ConvertRGBToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_YCbCR:
					DevLib::Graphics::ImageProcess::ConvertYCbCrToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_HSV:
					DevLib::Graphics::ImageProcess::ConvertHSVToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_BGRA:
					memcpy(m_xImage->data, image.GetImageConst(), image.GetSize());
					break;
				case DevLib::Graphics::ImageType::IMG_ABGR:
					DevLib::Graphics::ImageProcess::ConvertARGBToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_RGBA:
					DevLib::Graphics::ImageProcess::ConvertRGBAToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
					break;
				case DevLib::Graphics::ImageType::IMG_ARGB:
					DevLib::Graphics::ImageProcess::ConvertARGBToBGRA(image.GetImageConst(), image.GetWidth(), image.GetHeight(), reinterpret_cast<uint8_t *>(m_xImage->data));
				default:
				;
				}
			}

			RedrawWindow();
#endif
		}

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
		void CImageViewer::OnPaintEvent(GUI::WinDC &dc)
		{
			if (m_xImage && m_image.GetWidth() && m_image.GetWidth())
			{
				XPutImage(GetDC().display, GetDC().window, DefaultGC(GetDC().display, 0), m_xImage,
						  0, 0, 0, 0, m_image.GetWidth(), m_image.GetHeight());
			}
		}
#endif

	}
}