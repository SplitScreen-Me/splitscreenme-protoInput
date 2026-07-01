#include "GetCursorInfoHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "WindowMsgHook.h"
namespace Proto
{

	BOOL WINAPI Hook_GetCursorInfo(PCURSORINFO pci)
	{
		if (GetCursorInfo(pci))
		{
		
			const auto& state = FakeMouseKeyboard::GetMouseState();
			POINT clientPos = { state.x, state.y };
			if (FakeMouseKeyboard::PutMouseInsideWindow)
			{
				int clientWidth = HwndSelector::windowWidth;
				int clientHeight = HwndSelector::windowHeight;
				if (!FakeMouseKeyboard::DefaultTopLeftMouseBounds)
				{
					if (clientPos.y < 1)
						clientPos.y = 0;  // Top edge
					if (clientPos.x < 1)
						clientPos.x = 0;  // Left edge
				}
				if (!FakeMouseKeyboard::DefaultBottomRightMouseBounds)
				{
					if (clientPos.y > clientHeight - 1)
						clientPos.y = clientHeight - 1;  // Bottom edge
					if (clientPos.x > clientWidth - 1)
						clientPos.x = clientWidth - 1;  // Right edge
				}
			}

			//any scaling?
			clientPos = WindowMsgHook::getfactor(clientPos);

			ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), &clientPos);
			pci->ptScreenPos.x = clientPos.x;
			pci->ptScreenPos.y = clientPos.y;
			
		}
		return true;

		
	}

	void GetCursorInfoHook::ShowGuiStatus()
	{

	}

	void GetCursorInfoHook::InstallImpl()
	{
		hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetCursorInfo", Hook_GetCursorInfo));
	}

	void GetCursorInfoHook::UninstallImpl()
	{
		UninstallHook(&hookInfo);
	}

}
