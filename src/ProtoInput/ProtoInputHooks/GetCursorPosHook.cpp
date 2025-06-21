#include "GetCursorPosHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"

namespace Proto
{

BOOL WINAPI Hook_GetCursorPos(LPPOINT lpPoint)
{	
	if (lpPoint)
	{
		const auto& state = FakeMouseKeyboard::GetMouseState();
		lpPoint->x = state.x;
		lpPoint->y = state.y;

		if (FakeMouseKeyboard::PutMouseInsideWindow)
		{
			int clientWidth = HwndSelector::windowWidth;
			int clientHeight = HwndSelector::windowHeight;
			if (!FakeMouseKeyboard::DefaultTopLeftMouseBounds)
			{
				if (lpPoint->y < 1)
					lpPoint->y = 0;  // Top edge
				if (lpPoint->x < 1)
					lpPoint->x = 0;  // Left edge
			}
			if (!FakeMouseKeyboard::DefaultBottomRightMouseBounds)
			{
				if (lpPoint->y > clientHeight - 1)
					lpPoint->y = clientHeight - 1;  // Bottom edge
				if (lpPoint->x > clientWidth - 1)
					lpPoint->x = clientWidth - 1;  // Right edge
			}
		}
		ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), lpPoint);
	}
	
	return true;
}

void GetCursorPosHook::ShowGuiStatus()
{
	
}

void GetCursorPosHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetCursorPos", Hook_GetCursorPos));
}

void GetCursorPosHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
