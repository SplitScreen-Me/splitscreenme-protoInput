
#include <imgui.h>
#include "HwndSelector.h"
#include "FakeMouseKeyboard.h"
#include "SetWindowsHookHook.h"

namespace Proto
{
	HOOKPROC SetWindowsHookHook::gameshookcallLLKB = nullptr;
	HOOKPROC SetWindowsHookHook::gameshookcallLLMouse = nullptr;
	bool SetWindowsHookHook::LLKBhooked = false;
	bool SetWindowsHookHook::LLMousehooked = false;

	HHOOK WINAPI SetWindowsHookExAHook(
		int       idHook,
		HOOKPROC  lpfn,
		HINSTANCE hmod,
		DWORD     dwThreadId
	) 
	{
		if (idHook == WH_KEYBOARD_LL)
		{ 
			FakeMouseKeyboard::CallgameWindowLLKBHooks = true;
			SetWindowsHookHook::gameshookcallLLKB = lpfn;
		}
		else if (idHook == WH_MOUSE_LL)
		{
			FakeMouseKeyboard::CallgameWindowLLMouseHooks = true;
			SetWindowsHookHook::gameshookcallLLMouse = lpfn;
		}

		return (HHOOK)0x1;
	}

	LRESULT CallNextHookExHook(
		HHOOK  hhk,
		int    nCode,
		WPARAM wParam,
		LPARAM lParam
	) {
		return 0;
	}

	void SetWindowsHookHook::FireFakeLLMouseMove(int x, int y)
	{

		MSLLHOOKSTRUCT ms{};
		ms.pt.x = x;
		ms.pt.y = y;
		ms.mouseData = 0;
		ms.flags = 0;
		ms.time = GetTickCount();
		ms.dwExtraInfo = 0;

		gameshookcallLLMouse(HC_ACTION, WM_MOUSEMOVE, (LPARAM)&ms);
	}

	void SetWindowsHookHook::FireFakeLLKeyboardEvent(int vk, bool pressed)
	{
		if (vk == VK_CONTROL)
			vk = VK_LCONTROL;
		KBDLLHOOKSTRUCT kbd{};
		kbd.vkCode = vk;
		kbd.scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
		kbd.flags = pressed ? 0 : LLKHF_UP;
		kbd.time = GetTickCount();
		kbd.dwExtraInfo = 0;

		WPARAM msg = pressed ? WM_KEYDOWN : WM_KEYUP;

		SetWindowsHookHook::gameshookcallLLKB(HC_ACTION, msg, (LPARAM)&kbd); //games callback keyboard events
	}

	void SetWindowsHookHook::ShowGuiStatus()
	{
		ImGui::Text("Input actions for Scanoption. 0 is move+click. 1 is only move. 2 is only click");
	}

	void SetWindowsHookHook::InstallImpl()
	{
		hookInfoWndHookExA = std::get<1>(InstallNamedHook(L"user32", "SetWindowsHookExA", SetWindowsHookExAHook));
		hookInfoNextHookEx = std::get<1>(InstallNamedHook(L"user32", "CallNextHookEx", CallNextHookExHook));
	}
	
	void SetWindowsHookHook::UninstallImpl()
	{
		UninstallHook(&hookInfoWndHookExA);
	}

}
