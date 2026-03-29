#include "GetKeyboardStateHook.h"
#include "FakeMouseKeyboard.h"
#include "XinputHook.h"

namespace Proto
{

BOOL WINAPI Hook_GetKeyboardState(PBYTE lpKeyState)
{
	if (lpKeyState)
	{
		memset(lpKeyState, 0, 256);

		for (int vkey = 0; vkey < 256; ++vkey)
		{
			if (!XinputHook::TranslateMKBtoXinput)
				lpKeyState[vkey] = FakeMouseKeyboard::IsKeyStatePressed(vkey) ? 0b10000000 : 0;
			else lpKeyState[vkey] = 0;
		}
	}

	return TRUE;
}


void GetKeyboardStateHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetKeyboardState", Hook_GetKeyboardState));
}

void GetKeyboardStateHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
