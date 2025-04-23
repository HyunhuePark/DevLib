#pragma once

#include "../Base/DevLibTypes.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 5105)
#include <Windows.h>
#pragma warning(default : 5105)
#elif defined(__linux__) // Linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#endif

namespace DevLib
{
	namespace GUI
	{

#if defined(_MSC_VER)

		using WinHandle = HWND;
		using WinDC = HDC;

		using WinResult = LRESULT;
		using WinMessage = UINT;
		using WinWParam = WPARAM;
		using WinLParam = LPARAM;

#elif defined(__linux__) // Linux
		using WinDC = struct WinDC
		{
			Window window;
			XVisualInfo* visualInfo;
			Display* display;
		};
#endif

		// Mouse Event Type
		enum class MouseEvent
		{
			LButtonDown,
			LButtonUp,
			RButtonDown,
			RButtonUP,
			MButtonDown,
			MButtonUP,
			WheelUp,
			WheelDown,
			Move,
			Error
		};

		// Mouse Position Struct
		using MousePosition = struct MousePosition
		{
			int32_t x;
			int32_t y;
		};
	}
}
