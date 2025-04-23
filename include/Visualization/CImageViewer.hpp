#pragma once

#include "../GUI/CWindow.hpp"
#include "../Graphics/CImage.hpp"
#include "../Base/CEvent.hpp"

namespace DevLib
{
	namespace Visualization
	{
		class CImageViewer : public GUI::CDevLibWindow
		{
		public:
			CImageViewer(std::string title = std::string("RoutineImageViewer"));
			~CImageViewer();

			void ShowImage(const Graphics::CImageObject& image);
			void ShowImage(const std::string& image_path);
			void ShowImage(int32_t width, int32_t height, const uint8_t* img, Graphics::ImageType type = Graphics::ImageType::IMG_BGR);

		private:
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
			Graphics::CImage m_image;
			XImage* m_xImage;
			void OnPaintEvent(GUI::WinDC& dc) override;
#endif
		};
	}
}