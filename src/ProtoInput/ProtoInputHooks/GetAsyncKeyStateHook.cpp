#include "GetAsyncKeyStateHook.h"
#include "FakeMouseKeyboard.h"
#include "XinputHook.h"

namespace Proto
{

SHORT WINAPI Hook_GetAsyncKeyState(int vKey)
{	

	auto ret = (FakeMouseKeyboard::IsKeyStatePressed(vKey) ? 0b1000000000000000 : 0) | (FakeMouseKeyboard::IsAsyncKeyStatePressed(vKey) ? 1 : 0);
	FakeMouseKeyboard::ClearAsyncKeyState(vKey);
	if (!XinputHook::TranslateMKBtoXinput)
	{
		return ret;
	}
	else return 0;
	
}

void GetAsyncKeyStateHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetAsyncKeyState", Hook_GetAsyncKeyState));
}

void GetAsyncKeyStateHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
