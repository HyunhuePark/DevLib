#include "../../include/GUI/CWindow.hpp"

namespace DevLib
{
	namespace GUI
	{
#if defined(_MSC_VER)
		LRESULT CALLBACK RoutineWindowProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
		{
			const std::string propName = "DevLibWindow_" + std::to_string(reinterpret_cast<size_t>(hWnd));

			const auto pThis = static_cast<CDevLibWindow*>(GetPropA(hWnd, propName.data()));

			if (pThis)
			{
				pThis->PreTranslate(hWnd, iMessage, wParam, lParam);
			}

			return DefWindowProc(hWnd, iMessage, wParam, lParam);
		}
#endif

		CDevLibWindow::CDevLibWindow(std::string wndClassName)
			: m_cursorX(0), m_cursorY(0), m_strWindowTitle(std::move(wndClassName))
#if defined(_MSC_VER)
			, m_hdc(nullptr), m_hWnd(nullptr)
#elif defined(__linux__) // Linux
			, m_hdc({})
#endif
		{
			// SetDefault Callback
			RegisterCallbackMouseEvent(&CDevLibWindow::OnMouseEvent, this);
			RegisterCallbackMouseWheelEvent(&CDevLibWindow::OnMouseWheelEvent, this);
			RegisterCallbackKeyEvent(&CDevLibWindow::OnKeyEvent, this);
			RegisterCallbackPaintEvent(&CDevLibWindow::OnPaintEvent, this);
			RegisterCallbackResizeEvent(&CDevLibWindow::OnResizeEvent, this);

			// Create Event
			m_eKey.Create();
			m_eMouse.Create();
			m_eThread.Create();

			m_threadMessage.StartThread(&CDevLibWindow::MessageLoop, this);

			(void)m_eThread.WaitForEvent();
		}

		CDevLibWindow::~CDevLibWindow()
		{
#if defined(_MSC_VER)
			const std::string propName = "DevLibWindow_" + std::to_string(reinterpret_cast<size_t>(m_hWnd));
			RemovePropA(m_hWnd, propName.data());

			if (m_hWnd)
			{
				m_threadMessage.StopThread();

				PostMessage(m_hWnd, WM_CLOSE, 0, 0);

				m_threadMessage.WaitForEndThread();

				// Close DC
				ReleaseDC(m_hWnd, m_hdc);

				DestroyWindow(m_hWnd);

				m_hWnd = nullptr;
			}
#elif defined(__linux__) // Linux
			m_threadMessage.StopThread();
			m_threadMessage.WaitForEndThread();

			Hide();
			XDestroyWindow(m_hdc.display, m_hdc.window);
			XCloseDisplay(m_hdc.display);
			XFree(m_hdc.visualInfo);
#endif

			m_lastKeyCode = '\0';
			(void)m_eMouse.SetEvent();
			(void)m_eKey.SetEvent();
		}


		void CDevLibWindow::MessageLoop()
		{
#if defined(_MSC_VER)
			MSG message = {};

			// Create Window
			WNDCLASS WndClass;
			const auto hInstance = GetModuleHandle(nullptr);

			memset(&WndClass, 0, sizeof(WndClass));
			WndClass.cbClsExtra = 0;
			WndClass.cbWndExtra = 0;
			WndClass.hbrBackground = nullptr;
			WndClass.hCursor = nullptr;
			WndClass.hInstance = hInstance;
			WndClass.lpfnWndProc = RoutineWindowProc;
			WndClass.lpszClassName = "DevLibWindow";
			WndClass.style = NULL;

			RegisterClass(&WndClass);

			m_hWnd = CreateWindowA("DevLibWindow", m_strWindowTitle.c_str(),
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL,
				(HMENU)NULL,
				hInstance,
				NULL);

			m_hdc = ::GetDC(m_hWnd);
			ShowWindow(m_hWnd, SW_HIDE);

			// Set Prop
			const std::string propName = "DevLibWindow_" + std::to_string(reinterpret_cast<size_t>(m_hWnd));
			SetPropA(m_hWnd, propName.data(), this);

			(void)m_eThread.SetEvent();

			// Thread Run
			while (GetMessage(&message, m_hWnd, 0, 0) && m_threadMessage.IsRunThread())
			{

				TranslateMessage(&message);
				DispatchMessage(&message);
			}
#elif defined(__linux__) // Linux

			// XLib Thread
			XInitThreads();

			// Display
			m_hdc.display = XOpenDisplay(nullptr);

			// VisualInfo
			GLint attribute[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
			m_hdc.visualInfo = glXChooseVisual(m_hdc.display, DefaultScreen(m_hdc.display), attribute);

			Colormap colorMap = XCreateColormap(m_hdc.display, RootWindow(m_hdc.display, m_hdc.visualInfo->screen), m_hdc.visualInfo->visual, AllocNone);
			XSetWindowAttributes swa;
			swa.colormap = colorMap;
			swa.event_mask = KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | StructureNotifyMask;
			m_hdc.window = XCreateWindow(m_hdc.display, RootWindow(m_hdc.display, m_hdc.visualInfo->screen), 0, 0, 800, 600, 0,
				m_hdc.visualInfo->depth, InputOutput, m_hdc.visualInfo->visual, CWColormap | CWEventMask, &swa);

			// Hook DeleteWindow
			Atom wmDelete = XInternAtom(m_hdc.display, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(m_hdc.display, m_hdc.window, &wmDelete, 1);

			(void)m_eThread.SetEvent();

			SetTitle(m_strWindowTitle);

			XEvent xe;
			while (m_threadMessage.IsRunThread())
			{
				XNextEvent(m_hdc.display, &xe);

				switch (xe.type)
				{
				case ConfigureNotify:
					m_callbackResizeEvent(xe.xconfigure.width, xe.xconfigure.height);
					break;
				case ClientMessage:
					Hide();
					break;
				case Expose:
					m_callbackPaintEvent(m_hdc);
					break;
				case KeyPress:
					m_lastKeyCode = XLookupKeysym(&xe.xkey, 0);
					m_callbackKeyEvent(m_lastKeyCode);
					(void)m_eKey.SetEvent();
					break;
				case ButtonPress:
					switch (xe.xbutton.button)
					{
					case 1: // Left Button
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::LButtonDown;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					case 2: // Wheel Button
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::MButtonDown;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					case 3: // Right Button
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::RButtonDown;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					case 4: // Wheel UP
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::WheelUp;

						m_callbackMouseWheelEvent(127);

						(void)m_eMouse.SetEvent();
						break;

					case 5: // Wheel Down
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::WheelDown;

						m_callbackMouseWheelEvent(-127);

						(void)m_eMouse.SetEvent();
						break;

					default:
						break;
					}
					break;

				case ButtonRelease:
					switch (xe.xbutton.button)
					{
					case 1:
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::LButtonUp;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					case 2:
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::MButtonUP;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					case 3:
						m_cursorX = xe.xbutton.x;
						m_cursorY = xe.xbutton.y;
						m_mouseEvent = MouseEvent::RButtonUP;

						m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
						(void)m_eMouse.SetEvent();
						break;

					}
					break;

				case MotionNotify:
					m_mouseEvent = MouseEvent::Move;
					m_cursorX = xe.xbutton.x;
					m_cursorY = xe.xbutton.y;

					m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
					(void)m_eMouse.SetEvent();
					break;
				}
			}

#endif
		}

#if defined(_MSC_VER)
		void CDevLibWindow::PreTranslate(WinHandle hWnd, WinMessage iMessage, WinWParam wParam, WinLParam lParam)
		{
			// PreTransLateMessage
			switch (iMessage)
			{
			case WM_KEYDOWN:
				m_lastKeyCode = static_cast<char>(wParam);

				m_callbackKeyEvent(m_lastKeyCode);
				(void)m_eKey.SetEvent();
				break;
			case WM_MOUSEWHEEL:
				m_callbackMouseWheelEvent(HIWORD(wParam));
				break;

			case WM_LBUTTONDOWN:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::LButtonDown;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_LBUTTONUP:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::LButtonUp;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_RBUTTONDOWN:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::RButtonDown;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_RBUTTONUP:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::RButtonUP;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_MBUTTONDOWN:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::MButtonDown;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_MBUTTONUP:
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);
				m_mouseEvent = MouseEvent::MButtonUP;

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;
			case WM_MOUSEMOVE:
				m_mouseEvent = MouseEvent::Move;
				m_cursorY = HIWORD(lParam);
				m_cursorX = LOWORD(lParam);

				m_callbackMouseEvent(m_mouseEvent, m_cursorX, m_cursorY);
				(void)m_eMouse.SetEvent();
				break;

			case WM_SIZE:
				m_callbackResizeEvent(LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_PAINT:
				m_callbackPaintEvent(m_hdc);
				break;

			case WM_NULL:	// 0
			case WM_CLOSE:
				m_threadMessage.StopThread();
				break;
			default:
				break;
			}
		}
#endif

		void CDevLibWindow::SetTitle(const std::string& title) const
		{
#if defined(_MSC_VER)
			::SetWindowTextA(m_hWnd, title.c_str());
#elif defined(__linux__) // Linux
			XStoreName(m_hdc.display, m_hdc.window, title.c_str());
#endif
		}

		uint8_t CDevLibWindow::WaitForKey(const uint32_t waitMillisecond /*= 0xFFFFFFFF*/) const
		{
#if defined(_MSC_VER)
			if (!IsWindow(m_hWnd)) return '\0';
#elif defined(__linux__) // Linux
			if (m_hdc.display == nullptr) return '\0';
#endif
			(void)m_eKey.WaitForEvent(waitMillisecond);
			return m_lastKeyCode;
		}

		MouseEvent CDevLibWindow::WaitForMouse() const
		{
#if defined(_MSC_VER)
			if (!IsWindow(m_hWnd)) return MouseEvent::Error;
#elif defined(__linux__) // Linux
			if (m_hdc.display == nullptr) return MouseEvent::Error;
#endif
			(void)m_eMouse.WaitForEvent();
			return m_mouseEvent;
		}

		MousePosition CDevLibWindow::GetMousePosition() const
		{
			const MousePosition pos = { m_cursorX, m_cursorY };
			return pos;
		}

		void CDevLibWindow::Show()
		{
			if (m_bVisible == false)
			{
#if defined(_MSC_VER)
				ShowWindow(m_hWnd, SW_SHOW);
#elif defined(__linux__) // Linux

				XMapWindow(m_hdc.display, m_hdc.window);
				XFlush(m_hdc.display);
#endif
				m_bVisible = true;
			}
		}

		void CDevLibWindow::Hide()
		{
			if (m_bVisible == true)
			{
#if defined(_MSC_VER)
				ShowWindow(m_hWnd, SW_HIDE);
#elif defined(__linux__) // Linux

				XUnmapWindow(m_hdc.display, m_hdc.window);
				XFlush(m_hdc.display);

#endif
				m_bVisible = false;
			}
		}

		void CDevLibWindow::SetResize(uint32_t width, uint32_t height) const
		{
#if defined(_MSC_VER)
			RECT pos, rt;
			GetWindowRect(m_hWnd, &pos);
			GetClientRect(m_hWnd, &rt);
			MoveWindow(m_hWnd, pos.left, pos.top, static_cast<decltype(pos.right)>(width) + ((pos.right - pos.left) - rt.right), static_cast<decltype(pos.right)>(height) + ((pos.bottom - pos.top) - rt.bottom), TRUE);
#elif defined(__linux__) // Linux
			XResizeWindow(m_hdc.display, m_hdc.window, width, height);
#endif
		}

		WinDC& CDevLibWindow::GetDC()
		{
			return m_hdc;
		}

		void CDevLibWindow::RedrawWindow()
		{
#if defined(_MSC_VER)
			Show();
			PostMessage(m_hWnd, WM_PAINT, NULL, NULL);
#elif defined(__linux__) // Linux
			XEvent exppp;
			exppp.type = Expose;
			exppp.xexpose.window = m_hdc.window;
			XSendEvent(m_hdc.display, m_hdc.window, False, ExposureMask, &exppp);
			XFlush(m_hdc.display);
#endif
		}
	}
}