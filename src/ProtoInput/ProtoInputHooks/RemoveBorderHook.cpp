#include "RemoveBorderHook.h"
#include "Gui.h"
#include "HwndSelector.h"
#include "FakeCursor.h"

namespace Proto
{

	inline void FilterStyle(LONG& style)
	{
		style &= ~(WS_BORDER | WS_SYSMENU | WS_DLGFRAME | WS_CAPTION |
			WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		style |= WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	inline void FilterStylePtr(LONG_PTR& style)
	{
		style &= ~(WS_BORDER | WS_SYSMENU | WS_DLGFRAME | WS_CAPTION |
			WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		style |= WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	BOOL CALLBACK EnumWindowsProc(HWND handle, LPARAM lParam)
	{
		DWORD processId = GetCurrentProcessId();
		DWORD windowProcessId;
		GetWindowThreadProcessId(handle, &windowProcessId);

		if (windowProcessId == processId &&
			GetWindow(handle, GW_OWNER) == (HWND)0 &&
			IsWindowVisible(handle) &&
			handle != (HWND)Proto::ConsoleHwnd &&
			handle != Proto::ProtoGuiHwnd &&
			handle != FakeCursor::GetPointerWindow())
		{
			*(HWND*)lParam = handle; // Store the found window handle
			return FALSE; // Stop enumeration
		}
		return TRUE; // Continue enumeration
	}

	HWND GetMainWindow()
	{
		HWND mainWindow = NULL;
		EnumWindows(EnumWindowsProc, (LPARAM)&mainWindow);
		return mainWindow;
	}

	void RemoveWindowBorder(HWND hWnd)
	{
		if (!hWnd) return;

		RECT windowRect, clientRect;
		GetWindowRect(hWnd, &windowRect);
		GetClientRect(hWnd, &clientRect);

		POINT clientTopLeft = { clientRect.left, clientRect.top };
		ClientToScreen(hWnd, &clientTopLeft);

		int clientWidth = clientRect.right - clientRect.left;
		int clientHeight = clientRect.bottom - clientRect.top;

		LONG currentStyle = GetWindowLong(hWnd, GWL_STYLE);
		LONG newStyle = currentStyle;
		FilterStyle(newStyle);

		RECT newWindowRect = { 0, 0, clientWidth, clientHeight };
		AdjustWindowRectEx(&newWindowRect, newStyle, FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));

		SetWindowLong(hWnd, GWL_STYLE, newStyle);

		SetWindowPos(hWnd,
			NULL,
			clientTopLeft.x, // Position where the client area was
			clientTopLeft.y,
			newWindowRect.right - newWindowRect.left,
			newWindowRect.bottom - newWindowRect.top,
			SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	LONG WINAPI AltHook_SetWindowLongA(
		HWND hWnd,
		int  nIndex,
		LONG dwNewLong
	)
	{
		if (nIndex == GWL_STYLE)
		{
			FilterStyle(dwNewLong);
		}
		return SetWindowLongA(hWnd, nIndex, dwNewLong);
	}

	LONG WINAPI AltHook_SetWindowLongW(
		HWND hWnd,
		int  nIndex,
		LONG dwNewLong
	)
	{
		if (nIndex == GWL_STYLE)
		{
			FilterStyle(dwNewLong);
		}
		return SetWindowLongW(hWnd, nIndex, dwNewLong);
	}

	LONG_PTR WINAPI AltHook_SetWindowLongPtrA(
		HWND hWnd,
		int  nIndex,
		LONG_PTR dwNewLong
	)
	{
		if (nIndex == GWL_STYLE)
		{
			FilterStylePtr(dwNewLong);
		}
		return SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
	}

	LONG_PTR WINAPI AltHook_SetWindowLongPtrW(
		HWND hWnd,
		int  nIndex,
		LONG_PTR dwNewLong
	)
	{
		if (nIndex == GWL_STYLE)
		{
			FilterStylePtr(dwNewLong);
		}
		return SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
	}

	void RemoveBorderHook::InstallImpl()
	{
		hookInfoA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongA", AltHook_SetWindowLongA));
		hookInfoW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongW", AltHook_SetWindowLongW));
		hookInfoPtrA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrA", AltHook_SetWindowLongPtrA));
		hookInfoPtrW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrW", AltHook_SetWindowLongPtrW));

		if (HWND hWnd = GetMainWindow())
		{
			RemoveWindowBorder(hWnd);
		}
	}

	void RemoveBorderHook::UninstallImpl()
	{
		UninstallHook(&hookInfoA);
		UninstallHook(&hookInfoW);
		UninstallHook(&hookInfoPtrA);
		UninstallHook(&hookInfoPtrW);
	}

}
