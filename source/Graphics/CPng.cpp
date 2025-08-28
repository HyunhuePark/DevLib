#include "../../include/Graphics/CPng.hpp"

#include "../../include/IO/CFile.hpp"
#include "../../include/Graphics/CImage.hpp"

#include <png.h>
#include <cstring>

#if defined(_MSC_VER)
#pragma warning(disable : 5105)

#elif defined(__linux__) // Linux
#endif

namespace DevLib {
	namespace Graphics {
		CPng::CPng()
			= default;

		CPng::CPng(const CPng &src)
		{
			*this = src;
		}

		CPng::CPng(const uint8_t* pngData, const uint32_t size)
		{
			LoadImageMem(pngData, size);
		}

		CPng::CPng(const std::string& imgPath)
		{
			LoadImageFile(imgPath);
		}

		CPng::CPng(const DevLib::Graphics::CImageObject& img)
		{
			FromImage(img);
		}

		CPng &CPng::operator=(const CPng &src)
		{
			m_png.clear();

			m_width = src.m_width;
			m_height = src.m_height;
			m_channels = src.m_channels;

			m_png.assign(src.m_png.begin(), src.m_png.end());

			return *this;
		}


		CPng::~CPng()
		{
			m_png.clear();
		}

		bool CPng::LoadImageFile(const std::string& imgPath)
		{
			bool bRet = false;

			IO::CFile pngFile;
			if (pngFile.Open(imgPath, "rb"))
			{
				if (pngFile.GetFileSize() > 0)
				{
					// Read File
					m_png.resize(pngFile.GetFileSize());
					pngFile.Read(m_png.data(), m_png.size());
					pngFile.Close();


					// header parse
					png_structp png_ptr;
					png_infop png_info;

					png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

					if (png_ptr)
					{
						png_info = png_create_info_struct(png_ptr);
						if (png_info)
						{
							if (!setjmp(png_jmpbuf(png_ptr))) 
							{
								png_set_read_fn(png_ptr, this, reinterpret_cast<png_rw_ptr>(&CPng::PngRead));

								png_read_info(png_ptr, png_info);

								m_width = png_get_image_width(png_ptr, png_info); // 이미지의 너비
								m_height = png_get_image_height(png_ptr, png_info); // 이미지의 높이
								const int color_type = png_get_color_type(png_ptr, png_info);

								switch (color_type)
								{
								case PNG_COLOR_TYPE_GRAY:		m_channels = 1;	break;
								case PNG_COLOR_TYPE_GRAY_ALPHA:	m_channels = 2; break;
								case PNG_COLOR_TYPE_PALETTE:	
								case PNG_COLOR_TYPE_RGB:		m_channels = 3; break;
								case PNG_COLOR_TYPE_RGB_ALPHA:	m_channels = 4;	break;
								default:;
								}
								bRet = true;
							}
							png_destroy_info_struct(png_ptr, &png_info);
							png_destroy_read_struct(&png_ptr, &png_info, nullptr);
						}
					}
				}
				else
				{
					// 오류 처리
					m_png.clear();
				}
			}

			return bRet;
		}

		bool CPng::LoadImageMem(const uint8_t* pngData, const uint32_t size)
		{
			bool bRet = false;

			// Copy mem
			m_png.resize(size);
			std::memcpy(m_png.data(), pngData, m_png.size());

			png_structp png_ptr;
			png_infop png_info;

			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

			if (png_ptr)
			{
				png_info = png_create_info_struct(png_ptr);
				if (png_info)
				{
					if (!setjmp(png_jmpbuf(png_ptr)))
					{
						// Header Parse
						png_set_read_fn(png_ptr, this, reinterpret_cast<png_rw_ptr>(&CPng::PngRead));

						png_read_info(png_ptr, png_info);

						m_width = png_get_image_width(png_ptr, png_info); // 이미지의 너비
						m_height = png_get_image_height(png_ptr, png_info); // 이미지의 높이
						const int color_type = png_get_color_type(png_ptr, png_info);

						switch (color_type)
						{
						case PNG_COLOR_TYPE_GRAY:		m_channels = 1;	break;
						case PNG_COLOR_TYPE_GRAY_ALPHA:	m_channels = 2; break;
						case PNG_COLOR_TYPE_PALETTE:
						case PNG_COLOR_TYPE_RGB:		m_channels = 3; break;
						case PNG_COLOR_TYPE_RGB_ALPHA:	m_channels = 4;	break;
						default:;
						}
						bRet = true;
					}
					png_destroy_info_struct(png_ptr, &png_info);
					png_destroy_read_struct(&png_ptr, &png_info, nullptr);
				}
			}

			return bRet;
		}

		bool CPng::SaveImageFile(const std::string& imgPath) const
		{
			bool bRet = false;

			IO::CFile file;
			if (file.Open(imgPath, "wb"))
			{
				(void)file.Write(m_png.data(), m_png.size());
				file.Close();

				bRet = true;
			}

			return bRet;
		}

		bool CPng::FromImage(const CImageObject& img)
		{
			CImage saveRGB(img);

			// PNG
			png_structp png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
			png_infop png_info = png_create_info_struct(png_ptr);

			int color = -1;
			switch (saveRGB.GetImageType())
			{
			case ImageType::IMG_GRAY:	color = PNG_FORMAT_GRAY; break;
			case ImageType::IMG_BGR555:	break;
			case ImageType::IMG_BGR565:	break;
			case ImageType::IMG_BGR:	saveRGB.ColorConvert(ImageType::IMG_RGB);  color = PNG_FORMAT_RGB; break;
			case ImageType::IMG_RGB:	color = PNG_FORMAT_RGB; break;
			case ImageType::IMG_YCbCR:	break;
			case ImageType::IMG_RGBA:	color = PNG_COLOR_TYPE_RGB_ALPHA;	break;
			case ImageType::IMG_ARGB:	saveRGB.ColorConvert(ImageType::IMG_RGBA); color = PNG_COLOR_TYPE_RGB_ALPHA;	break;
			case ImageType::IMG_BGRA:	saveRGB.ColorConvert(ImageType::IMG_RGBA); color = PNG_COLOR_TYPE_RGB_ALPHA;	break;
			case ImageType::IMG_ABGR:	saveRGB.ColorConvert(ImageType::IMG_RGBA); color = PNG_COLOR_TYPE_RGB_ALPHA;	break;
			case ImageType::IMG_HSV:	break;
			case ImageType::IMG_NONE:	break;
			case ImageType::IMG_NV12:	break;
			default:;
			}

			if (color != -1)
			{
				// Write Callback
				png_set_write_fn(png_ptr, this, reinterpret_cast<png_rw_ptr>(&CPng::PngWrite), nullptr);

				// Set Head 
				png_set_IHDR(png_ptr, png_info, saveRGB.GetWidth(), saveRGB.GetHeight(), 8, color, 0, 0, 0);

				// Head Write
				png_write_info(png_ptr, png_info);

				// Encode
				unsigned char* data = saveRGB.GetImage();
				png_bytep ptr = nullptr;
				const int lineSize = saveRGB.GetWidthStep();
				const int height = saveRGB.GetHeight();
				for (int y = 0; y < height; y++)
				{
					ptr = &data[y * lineSize];
					png_write_rows(png_ptr, &ptr, 1);
				}

				png_destroy_info_struct(png_ptr, &png_info);
				png_destroy_write_struct(&png_ptr, &png_info);

				m_width = img.GetWidth();
				m_height = img.GetHeight();
				m_channels = img.GetChannels();
			}

			return !m_png.empty() ? true : false;
		}

		bool CPng::ToImage(CImageObject& img) const
		{
			bool bRet = false;

			png_image pngImage{};
			pngImage.version = PNG_IMAGE_VERSION;

			if (png_image_begin_read_from_memory(&pngImage, m_png.data(), m_png.size()))
			{
				ImageType color = ImageType::IMG_NONE;
				switch (pngImage.format)
				{
				case PNG_FORMAT_GRAY:	color = ImageType::IMG_GRAY;	break;
				case PNG_FORMAT_BGR:
				case PNG_FORMAT_RGB:
				case PNG_FORMAT_RGB_COLORMAP :
				case PNG_FORMAT_BGR_COLORMAP :
					color = ImageType::IMG_BGR;
					pngImage.format = PNG_FORMAT_BGR;	break;
				case PNG_FORMAT_RGBA:	color = ImageType::IMG_RGBA;	break;
				case PNG_FORMAT_BGRA:	color = ImageType::IMG_BGRA;	break;
				case PNG_FORMAT_ABGR:	color = ImageType::IMG_ABGR;	break;
				case PNG_FORMAT_ARGB:	color = ImageType::IMG_ARGB;	break;
				default:;
				}

				if (color != ImageType::IMG_NONE)
				{
					img.Destroy();
					img.Create(pngImage.width, pngImage.height, color);

					if (!png_image_finish_read(&pngImage, nullptr, img.GetImage(), 0, nullptr))
					{
						img.Destroy();
						png_image_free(&pngImage);
					}
				}
				else
				{
					png_image_free(&pngImage);
				}

				bRet = true;
			}
			return bRet;
		}

		uint32_t CPng::GetWidth() const
		{
			return m_width;
		}

		uint32_t CPng::GetHeight() const
		{
			return m_height;
		}

		uint32_t CPng::GetChannels() const
		{
			return m_channels;
		}

		uint32_t CPng::GetSize() const
		{
			return static_cast<uint32_t>(m_png.size());
		}

		std::vector<uint8_t>& CPng::PngData()
		{
			return  m_png;
		}

		void CPng::PngRead(handle_t pngInfo, uint8_t* pData, size_t length)
		{
			auto* pPng = static_cast<CPng*>(png_get_io_ptr(static_cast<png_structp>(pngInfo)));

			memcpy(pData, pPng->m_png.data() + pPng->m_readOffset, length);
			pPng->m_readOffset += length;
		}

		void CPng::PngWrite(handle_t pngInfo, const uint8_t* pData, size_t length)
		{
			auto* pPng = static_cast<CPng*>(png_get_io_ptr(static_cast<png_structp>(pngInfo)));

			const auto curSize = pPng->m_png.size();
			pPng->m_png.resize(curSize + length);

			memcpy(&pPng->m_png[curSize], pData, length);
		}
	}
}
