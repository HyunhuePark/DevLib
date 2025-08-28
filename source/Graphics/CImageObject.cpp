#include "../../include/Graphics/CImageObject.hpp"

#include <cstring>

namespace DevLib
{
	namespace Graphics
	{

		inline int32_t ImageTypeToChannels(ImageType type)
		{
			int32_t nChannel = 1;
			switch (type)
			{
			case ImageType::IMG_GRAY:
			case ImageType::IMG_NV12:
				nChannel = 1;
				break;
			case ImageType::IMG_BGR555:
			case ImageType::IMG_BGR565:
				nChannel = 2;
				break;
			case ImageType::IMG_BGR:
			case ImageType::IMG_RGB:
			case ImageType::IMG_YCbCR:
			case ImageType::IMG_HSV:
				nChannel = 3;
				break;
			case ImageType::IMG_BGRA:
			case ImageType::IMG_ABGR:
			case ImageType::IMG_RGBA:
			case ImageType::IMG_ARGB:
				nChannel = 4;
				break;
			case ImageType::IMG_NONE: 
				break;
			default:
				break;
			}

			return nChannel;
		}

		CImageObject::CImageObject()
			= default;

		CImageObject::CImageObject(int32_t width, int32_t height, ImageType type /*= ImageType::IMG_BGR*/)
			: m_width(width), m_height(height), m_nChannels(ImageTypeToChannels(type)), m_widthStep(width* ImageTypeToChannels(type)), m_imageType(type)
		{
			if (type == ImageType::IMG_NV12)
			{
				m_img.resize((width * height) + ((width * height) >> 1));
			}
			else
			{
				m_img.resize(width * height * ImageTypeToChannels(type));
			}
		}

		CImageObject::CImageObject(const CImageObject& src)
		{
			*this = src;
		}

		CImageObject::~CImageObject()
		{
			Destroy();
 		}

		bool CImageObject::Create(int32_t width, int32_t height, ImageType type /*= ImageType::IMG_BGR*/)
		{
			bool bRet = false;
			if (empty())
			{
				m_width = width;
				m_height = height;
				m_nChannels = ImageTypeToChannels(type);
				m_widthStep = width * ImageTypeToChannels(type);
				m_imageType = type;

				if (type == ImageType::IMG_NV12)
				{
					m_img.resize((width * height) + ((width * height)>>1));
				}
				else
				{
					m_img.resize(width * height * ImageTypeToChannels(type));
				}

				bRet = true;
			}

			return bRet;
		}

		void CImageObject::Destroy()
		{
			m_width = 0;
			m_height = 0;
			m_nChannels = 0;
			m_widthStep = 0;

			m_img.clear();
		}

		bool CImageObject::IsNull() const
		{
			return m_img.empty();
		}

		bool CImageObject::empty() const
		{
			return m_img.empty();
		}

		void CImageObject::Clear(uint8_t val /*= 0*/)
		{
			memset(m_img.data(), val, m_img.size());
		}

		CImageObject& CImageObject::operator=(const CImageObject& src)
		{
			Destroy();

			m_width = src.m_width;
			m_height = src.m_height;
			m_nChannels = src.m_nChannels;
			m_widthStep = src.m_widthStep;
			m_imageType = src.m_imageType;

			m_img.assign(src.m_img.begin(), src.m_img.end());

			return *this;
		}

	}
}