#include "RemoveBorderHook.h"
#include "Gui.h"
#include "HwndSelector.h"
#include "FakeCursor.h"

namespace Proto
{
	bool RemoveBorderHook::DontWaitWindowBorder = false;

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

	bool HasBorder(HWND hWnd)
	{
		if (!hWnd) return false;
		LONG style = GetWindowLong(hWnd, GWL_STYLE);

		return	(style & WS_BORDER)		||
				(style & WS_CAPTION)	||
				(style & WS_DLGFRAME)	||
				(style & WS_THICKFRAME);
	}

	void WaitForWindowBorder()
	{
		DWORD TimerStartTime = GetTickCount();
		const int TimerTimeout = 1000 * 10;
		const int CheckInterval = 200; // check window has border every 200 ms.

		while (GetTickCount() - TimerStartTime < TimerTimeout)
		{
			if (HWND hWnd = (HWND)HwndSelector::GetSelectedHwnd())
			{
				// if the window has border.
				if (hWnd && HasBorder(hWnd))
				{
					RemoveWindowBorder(hWnd);
					return;
				}
			}

			Sleep(CheckInterval);
		}
		// if the timeout is reached apply the border removal.
		if (HWND hWnd = (HWND)HwndSelector::GetSelectedHwnd())
		{
			RemoveWindowBorder(hWnd);
		}
	}

	DWORD WINAPI WaitForWindowBorderThreadStart(LPVOID lpParameter)
	{
		printf("WaitForWindowBorder thread start\n");
		Proto::AddThreadToACL(GetCurrentThreadId());
		WaitForWindowBorder();
		return 0;
	}

	void RemoveBorderHook::InstallImpl()
	{
		hookInfoA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongA", AltHook_SetWindowLongA));
		hookInfoW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongW", AltHook_SetWindowLongW));
		hookInfoPtrA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrA", AltHook_SetWindowLongPtrA));
		hookInfoPtrW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrW", AltHook_SetWindowLongPtrW));

		if (!DontWaitWindowBorder)
		{
			// Without a thread, RemoveWindowBorder function won't work correctly with startup injection.
			const auto threadHandle = CreateThread(nullptr, 0,
				(LPTHREAD_START_ROUTINE)WaitForWindowBorderThreadStart, GetModuleHandle(0), 0, 0);

			if (threadHandle != nullptr)
				CloseHandle(threadHandle);
		}
		else
		{
			if (HWND hWnd = (HWND)HwndSelector::GetSelectedHwnd())
			{
				RemoveWindowBorder(hWnd);
			}
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
