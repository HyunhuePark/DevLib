#include "../../include/Visualization/CPointCloudViewer.hpp"

#include "../../include/Base/CElapseTimer.hpp"

namespace DevLib
{
	namespace Visualization
	{

		CPointCloudViewer::CPointCloudViewer(std::string title)
			: CDevLibWindow(std::move(title))
		{
			Create(GetDC());
			SetZoom(1);
		}

		void CPointCloudViewer::OnResizeEvent(int width, int height)
		{
			ReSize();
			RedrawWindow();
		}

		void CPointCloudViewer::OnPaintEvent(GUI::WinDC&)
		{
			// Make the rendering context current
			if (BeginContext())
			{
				//DevLib::CElapseTimer t;

				ClearBuffer();

				// Draw Models
				DrawModels();

				// Swap our scene to the front
				SwapBuffer();

				// Allow other rendering context to co-exist
				(void)EndContext();

				//t.PrintElapseTime("DrawTime");
			}
		}

		void CPointCloudViewer::OnMouseWheelEvent(int16_t zDelta)
		{
			if (zDelta < 0)
			{
				Zoom(1.03f);
			}
			else if (zDelta > 0)
			{
				Zoom(0.97f);
			}

			RedrawWindow();
		}

		void CPointCloudViewer::ShowPointCloud(CPointCloud<>& points)
		{
			Show();

			ClearModels();

			GL::CGlModel model;
			GL::CGlObject obj;
			obj.Points().resize(points.GetPoints().size());
			obj.Points().assign(points.GetPoints().begin(), points.GetPoints().end());

			model.AddObject(obj);
			AddModel(model);

			RedrawWindow();
		}

		void CPointCloudViewer::ShowModel(const GL::CGlModel& model)
		{
			Show();

			ClearModels();

			AddModel(model);

			RedrawWindow();
		}

		void CPointCloudViewer::ShowModel(const std::vector<GL::CGlModel>& models)
		{
			Show();

			ClearModels();

			for( auto& model: models ) AddModel(model);

			RedrawWindow();
		}

		void CPointCloudViewer::SetScale(float scale)
		{
			CGlCore::SetZoom(scale);
		}

		void CPointCloudViewer::OnMouseEvent(const GUI::MouseEvent event, int32_t x, int32_t y)
		{
			switch (event)
			{
			case GUI::MouseEvent::LButtonDown:
				m_prevPosition = GUI::MousePosition{ x, y };
				m_bMouseDownL = true;
				break;
			case GUI::MouseEvent::LButtonUp:
				m_bMouseDownL = false;
				break;
			case GUI::MouseEvent::RButtonDown:
				m_prevPosition = GUI::MousePosition{ x, y };
				m_bMouseDownR = true;
				break;
			case GUI::MouseEvent::RButtonUP:
				m_bMouseDownR = false;
				break;
			case GUI::MouseEvent::MButtonDown:
				m_prevPosition = GUI::MousePosition{ x, y };
				m_bMouseDownM = true;
				break;
			case GUI::MouseEvent::MButtonUP:
				m_bMouseDownM = false;
				break;
			case GUI::MouseEvent::Move:
				if (m_bMouseDownL)
				{
					RotationLookAt(static_cast<float>(m_prevPosition.x - x), static_cast<float>(m_prevPosition.y - y));
					m_prevPosition = GUI::MousePosition{ x, y };
					RedrawWindow();
				}
				else if (m_bMouseDownM)
				{
					TranslateCamera(static_cast<float>(m_prevPosition.x - x), static_cast<float>(m_prevPosition.y - y));
					m_prevPosition = GUI::MousePosition{ x, y };
					RedrawWindow();
				}
				else if (m_bMouseDownR)
				{
					RotationCamera(static_cast<float>(m_prevPosition.x - x), static_cast<float>(m_prevPosition.y - y));
					m_prevPosition = GUI::MousePosition{ x, y };
					RedrawWindow();
				}
				break;
			case GUI::MouseEvent::Error:
				break;
			case GUI::MouseEvent::WheelUp: 
				break;
			case GUI::MouseEvent::WheelDown: 
				break;
			default:;
			}
		}

		void CPointCloudViewer::OnKeyEvent(uint8_t value)
		{

			switch (value)
			{
#if defined(_MSC_VER)
			case VK_NUMPAD2:
#elif defined(__linux__) // Linux
			case '2':
#endif
				Camera().SetCameraInfo(0, -GetZoomScale(), 0, 0, 0, 0, 0, 0, 1);
				RedrawWindow();
				break;
#if defined(_MSC_VER)
			case VK_NUMPAD4:
#elif defined(__linux__) // Linux
			case '4':
#endif
				Camera().SetCameraInfo(-GetZoomScale(), 0, 0, 0, 0, 0, 0, 0, 1);
				RedrawWindow();
				break;
#if defined(_MSC_VER)
			case VK_NUMPAD5:
#elif defined(__linux__) // Linux
			case '5':
#endif
				Camera().SetCameraInfo(0, 0, GetZoomScale(), 0, 0, 0, 0, 1, 0);
				RedrawWindow();
				break;
#if defined(_MSC_VER)
			case VK_NUMPAD6:
#elif defined(__linux__) // Linux
			case '6':
#endif
				Camera().SetCameraInfo(GetZoomScale(), 0, 0, 0, 0, 0, 0, 0, 1);
				RedrawWindow();
				break;
#if defined(_MSC_VER)
			case VK_NUMPAD8:
#elif defined(__linux__) // Linux
			case '8':
#endif
				Camera().SetCameraInfo(0, GetZoomScale(), 0, 0, 0, 0, 0, 0, 1);
				RedrawWindow();
				break;
			default:;
			}
		}

	}
}