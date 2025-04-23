#pragma once

#include <vector>

#include "../Base/DevLibTypes.hpp"

namespace DevLib
{
	namespace Graphics
	{

		enum class ImageType
		{
			IMG_NONE,
			IMG_GRAY,
			IMG_BGR555,
			IMG_BGR565,
			IMG_BGR,
			IMG_RGB,
			IMG_YCbCR,
			IMG_HSV,
			IMG_BGRA,
			IMG_ABGR,
			IMG_RGBA,
			IMG_ARGB,
			IMG_NV12,
		};

		class CImageObject
		{
		public:
			CImageObject();
			CImageObject(int32_t width, int32_t height, ImageType type = ImageType::IMG_BGR);
			CImageObject(const CImageObject& src);
			~CImageObject();

			bool Create(int32_t width, int32_t height, ImageType type = ImageType::IMG_BGR);
			void Destroy();

			inline uint8_t* GetImage();
			inline const uint8_t* GetImageConst() const;
			inline std::vector<uint8_t>& GetImageRef();

			bool IsNull() const;
			bool empty() const;

			void Clear(uint8_t val = 0);

			inline int32_t GetWidth() const;
			inline int32_t GetHeight() const;
			inline int32_t GetChannels() const;
			inline int32_t GetWidthStep() const;
			inline ImageType GetImageType() const;
			inline size_t GetSize() const;

			// operator
			CImageObject& operator=(const CImageObject& src);

		protected:
			int32_t			m_width{};				// Image Width
			int32_t			m_height{};				// Image Height
			int32_t			m_nChannels{};			// Image ColorSpace
			int32_t			m_widthStep{};			// Image LineWidth
			ImageType		m_imageType{ ImageType::IMG_NONE };

		private:
			std::vector<uint8_t> m_img;

			EnableSmartPointer(CImageObject)
		};

		inline int32_t CImageObject::GetWidth() const
		{
			return m_width;
		}

		inline int32_t CImageObject::GetHeight() const
		{
			return m_height;
		}

		inline int32_t CImageObject::GetChannels() const
		{
			return m_nChannels;
		}

		inline int32_t CImageObject::GetWidthStep() const
		{
			return m_widthStep;
		}

		inline ImageType CImageObject::GetImageType() const
		{
			return m_imageType;
		}

		inline size_t CImageObject::GetSize() const
		{
			return m_img.size();
		}

		inline uint8_t* CImageObject::GetImage()
		{
			return m_img.data();
		}

		inline const uint8_t* CImageObject::GetImageConst() const
		{
			return m_img.data();
		}

		inline std::vector<uint8_t>& CImageObject::GetImageRef()
		{
			return m_img;
		}



	}
}