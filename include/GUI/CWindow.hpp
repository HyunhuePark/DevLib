#pragma once

#include "../Base/CThread.hpp"
#include "../Base/CEvent.hpp"
#include "../Base/DevLibCallback.hpp"
#include "DevLibGUITypes.hpp"

namespace DevLib
{
	namespace GUI
	{
		class CDevLibWindow
		{
		public:
			CDevLibWindow(std::string wndClassName = std::string("DevLibWindow"));
			virtual ~CDevLibWindow();

			uint8_t WaitForKey(uint32_t waitMillisecond = 0xFFFFFFFF) const;
			MouseEvent WaitForMouse() const;

			MousePosition GetMousePosition() const;

			void SetTitle(const std::string& title) const;
			void Show();
			void Hide();
			void RedrawWindow();

			void SetResize(uint32_t width, uint32_t height) const;
			WinDC& GetDC();

		private:
			CEvent m_eKey;
			CEvent m_eMouse;
			CEvent m_eThread;

			MouseEvent m_mouseEvent{ MouseEvent::Error };
			int32_t m_cursorX;
			int32_t m_cursorY;

			uint8_t m_lastKeyCode{};

			CThread m_threadMessage;
			void MessageLoop();

			std::string m_strWindowTitle;
			WinDC	m_hdc;

			bool m_bVisible = false;

#if defined(_MSC_VER)
			WinHandle m_hWnd;

		public:
			void PreTranslate(WinHandle hWnd, WinMessage iMessage, WinWParam  wParam, WinLParam lParam);
#else // Linux
#endif

			EnableCallback(MouseEvent, MouseEvent event, int32_t x, int32_t y)
			EnableCallback(MouseWheelEvent, int16_t zDelta)
			EnableCallback(KeyEvent, uint8_t value)
			EnableCallback(PaintEvent, WinDC& dc)
			EnableCallback(ResizeEvent, int width, int height)
		};

	}
}
