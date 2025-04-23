#pragma once

#include "../GUI/CWindow.hpp"
#include "../Base/CThread.hpp"
#include "../Base/CEvent.hpp"
#include "../GL/CGlCore.hpp"

namespace DevLib
{
	namespace Visualization
	{
		class CPointCloudViewer : public GUI::CDevLibWindow, public GL::CGlCore
		{
		public:
			CPointCloudViewer(std::string title = "RoutinePointCloudViewer");

			void ShowPointCloud(CPointCloud<>& points);
			void ShowModel(const GL::CGlModel& model);

			void SetScale(float scale);

		private:
			void OnResizeEvent(int width, int height) override;
			void OnPaintEvent(GUI::WinDC&) override;
			void OnMouseWheelEvent(int16_t zDelta) override;
			void OnMouseEvent(GUI::MouseEvent event, int32_t x, int32_t y) override;
			void OnKeyEvent(uint8_t value) override;

			GUI::MousePosition m_prevPosition{};
			bool m_bMouseDownL{};
			bool m_bMouseDownM{};
			bool m_bMouseDownR{};

		};

	}
}
