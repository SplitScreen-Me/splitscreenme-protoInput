#include "GetCursorPosHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "WindowMsgHook.h"
#include "XinputHook.h"
#include "SetCursorPosHook.h"

namespace Proto
{

BOOL WINAPI Hook_GetCursorPos(LPPOINT lpPoint)
{	
	if (lpPoint)
	{
		if (XinputHook::TranslateMKBtoXinput)
		{
			lpPoint->x = SetCursorPosHook::mousesethere.x;
			lpPoint->y = SetCursorPosHook::mousesethere.y;

		}
		else
		{
			const auto& state = FakeMouseKeyboard::GetMouseState();
			lpPoint->x = state.x;
			lpPoint->y = state.y;
		}
		else
		{
			lpPoint->x = SetCursorPosHook::mousesethere.x;
			lpPoint->y = SetCursorPosHook::mousesethere.y;
		}

		//any scaling?
		POINT clientPos = { lpPoint->x, lpPoint->y };
		clientPos = WindowMsgHook::getfactor(clientPos);

			lpPoint->x = clientPos.x; 
			lpPoint->y = clientPos.y;
			ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), lpPoint);
		}
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
