#include "../../include/Graphics/CJpeg.hpp"

#include "../../include/IO/CFile.hpp"

#include <turbojpeg.h>
#include <cstring>


namespace DevLib {
	namespace Graphics {
		CJpeg::CJpeg()
		= default;

		CJpeg::CJpeg(const CJpeg &src)
		{
			*this = src;
		}

		CJpeg::CJpeg(const uint8_t* jpegData, const int32_t size)
		{
			LoadImageMem(jpegData, size);
		}

		CJpeg::CJpeg(const std::string& imgPath)
		{
			LoadImageFile(imgPath);
		}

		CJpeg::CJpeg(DevLib::Graphics::CImageObject& img, uint32_t quality )
		{
			FromImage(img, quality);
		}

		CJpeg &CJpeg::operator=(const CJpeg &src)
		{
			m_jpeg.clear();

			m_width = src.m_width;
			m_height = src.m_height;
			m_channels = src.m_channels;

			m_jpeg.assign(src.m_jpeg.begin(), src.m_jpeg.end());

			return *this;
		}

		CJpeg::~CJpeg()
		{
			m_jpeg.clear();
		}

		bool CJpeg::LoadImageFile(const std::string& imgPath)
		{
			bool bRet = false;

			IO::CFile jpegFile;
			if(jpegFile.Open(imgPath, "rb"))
			{
				if (jpegFile.GetFileSize() > 0)
				{
					m_jpeg.resize(jpegFile.GetFileSize());
					jpegFile.Read(m_jpeg.data(), m_jpeg.size());
					jpegFile.Close();

					bRet = LoadImageMem(m_jpeg.data(), m_jpeg.size());
				}
			}

			return bRet;
		}

		bool CJpeg::LoadImageMem(const uint8_t* jpegData, const int32_t size)
		{
			bool bRet = false;

			m_width = 0;
			m_height = 0;
			m_channels = 0;
			int jpegSubsamp = 0;
			int jpegColorspace = 0;

			tjhandle handle = tjInitDecompress();
			if (handle)
			{
				// Parsing Header
				if (tjDecompressHeader3(handle, m_jpeg.data(), m_jpeg.size(), &m_width, &m_height, &jpegSubsamp, &jpegColorspace) == 0)
				{
					switch (jpegColorspace)
					{
					case TJPF_RGB:
					case TJPF_BGR:
					case TJPF_RGBX:
					case TJPF_BGRX:
					case TJPF_XRGB:
					case TJPF_XBGR:
						m_channels = 3;
						break;
					case TJPF_GRAY:
						m_channels = 1;
						break;
					case TJPF_RGBA:
					case TJPF_BGRA:
					case TJPF_ARGB:
					case TJPF_ABGR:
						m_channels = 4;
						break;
					}
					
					bRet = true;
				}
				else
				{
					m_jpeg.clear();
					
				}

				tjDestroy(handle);
			}

			return bRet;
		}

		bool CJpeg::SaveImageFile(const std::string& imgPath) const
		{
			bool bRet = false;

			IO::CFile jpegFile;
			if (jpegFile.Open(imgPath, "wb"))
			{
				if (jpegFile.IsOpenFile() == false) return false;
				(void)jpegFile.Write(m_jpeg.data(), m_jpeg.size());
				jpegFile.Close();

				bRet = true;
			}

			return bRet;
		}

		bool CJpeg::FromImage(CImageObject& img, uint32_t quality, JPEGSubSampling samp)
		{
			// TurboJPEG Create handle
			tjhandle handle = tjInitCompress();
			if (handle)
			{
				// Out Image
				unsigned char *srcBuf = img.GetImage();
				int width = img.GetWidth();
				int height = img.GetHeight();
				int pitch = img.GetWidthStep(); // stride

				// Image Format
				int pixelFormat = -1;
				switch (img.GetImageType())
				{
				case ImageType::IMG_RGB:
					pixelFormat = TJPF_RGB;
					break;
				case ImageType::IMG_BGR:
					pixelFormat = TJPF_BGR;
					break;
				case ImageType::IMG_RGBA:
					pixelFormat = TJPF_RGBA;
					break;
				case ImageType::IMG_BGRA:
					pixelFormat = TJPF_BGRA;
					break;
				case ImageType::IMG_GRAY:
					pixelFormat = TJPF_GRAY;
					break;
				}

				if (pixelFormat != -1)
				{
					m_jpeg.resize(height * pitch);
					unsigned long jpegSize = 0;
					auto pDst = m_jpeg.data();

					int tSamp = TJSAMP_422;
					switch (samp)
					{
					case JPEGSubSampling::SUBSAMP_420 :
						tSamp = TJSAMP_420;
						break;
					case JPEGSubSampling::SUBSAMP_422 :
						tSamp = TJSAMP_422;
						break;
					case JPEGSubSampling::SUBSAMP_444 :
						tSamp = TJSAMP_444;
						break;
					}

					// Encode
					int flags = TJFLAG_FASTDCT | TJFLAG_NOREALLOC;
					if (tjCompress2(handle, srcBuf, width, pitch, height, pixelFormat, &pDst, &jpegSize, tSamp, quality, flags) == 0)
					{
						// Jpeg Size
						m_jpeg.resize(jpegSize);

						// Parse Header
						LoadImageMem(m_jpeg.data(), m_jpeg.size());						
					}

				}
			}

			tjDestroy(handle);

			return m_jpeg.empty() ? false : true;
		}

		bool CJpeg::ToImage(CImageObject &img) const
		{
			bool bRet = false;

			tjhandle handle = tjInitDecompress();

			if (handle)
			{
				int width = 0, height = 0, jpegSubsamp = 0, jpegColorspace = 0;

				// Parse
				if (tjDecompressHeader3(handle, m_jpeg.data(), m_jpeg.size(), &width, &height, &jpegSubsamp, &jpegColorspace) == 0)
				{
					// format
					int pixelFormat = -1;
					ImageType color = ImageType::IMG_BGR;

					switch (jpegColorspace)
					{
					case TJCS_GRAY:
						pixelFormat = TJPF_GRAY;
						color = ImageType::IMG_GRAY;
						break;
					case TJCS_RGB:
						pixelFormat = TJPF_RGB;
						color = ImageType::IMG_RGB;
						break;
					case TJCS_YCbCr:
						pixelFormat = TJPF_BGR;
						color = ImageType::IMG_BGR;
						break;
					}

					if( pixelFormat != -1 )
					{
						img.Destroy();
						img.Create(width, height, color);

						// Decode
						if (tjDecompress2(handle, m_jpeg.data(), m_jpeg.size(), img.GetImage(), width, img.GetWidthStep(), height, pixelFormat, TJFLAG_FASTDCT) == 0)
						{
							bRet = true;
						}
					}
				}
			}
			tjDestroy(handle);

			return bRet;
		}

		int32_t CJpeg::GetWidth() const
		{
			return m_width;
		}

		int32_t CJpeg::GetHeight() const
		{
			return m_height;
		}

		int32_t CJpeg::GetChannels() const
		{
			return m_channels;
		}

		uint32_t CJpeg::GetSize() const
		{
			return static_cast<uint32_t>(m_jpeg.size());
		}

		std::vector<uint8_t>& CJpeg::JpegData()
		{
			return std::ref(m_jpeg);
		}
	}
}
