
#include <imgui.h>
#include "HwndSelector.h"
#include "FakeMouseKeyboard.h"
#include "SetWindowsHookHook.h"

namespace Proto //oops need startup injection
{
	HOOKPROC SetWindowsHookHook::gameshookcallLLKB = nullptr;
	HOOKPROC SetWindowsHookHook::gameshookcallLLMouse = nullptr;

	HOOKPROC SetWindowsHookHook::gameshookcallMessage = nullptr;

	bool SetWindowsHookHook::LLKBhooked = false;
	bool SetWindowsHookHook::LLMousehooked = false;

	bool SetWindowsHookHook::Messagehooked = false;

	bool calledatall = false;
	HHOOK WINAPI SetWindowsHookExAHook(
		int       idHook,
		HOOKPROC  lpfn,
		HINSTANCE hmod,
		DWORD     dwThreadId
	) 
	{
		calledatall = true;
		if (idHook == WH_KEYBOARD_LL)
		{ 
			
			SetWindowsHookHook::LLKBhooked = true;
			SetWindowsHookHook::gameshookcallLLKB = lpfn;
		}

		else if (idHook == WH_MOUSE_LL)
		{
			SetWindowsHookHook::LLMousehooked = true;
			SetWindowsHookHook::gameshookcallLLMouse = lpfn;
		}

		else if (idHook == WH_GETMESSAGE)
		{
			SetWindowsHookHook::Messagehooked = true;
			SetWindowsHookHook::gameshookcallMessage = lpfn;
		}

		return (HHOOK)0x1;
	}

	HHOOK WINAPI SetWindowsHookExWHook(
		int       idHook,
		HOOKPROC  lpfn,
		HINSTANCE hmod,
		DWORD     dwThreadId
	)
	{
		calledatall = true;
		if (idHook == WH_KEYBOARD_LL)
		{
			SetWindowsHookHook::LLKBhooked = true;
			SetWindowsHookHook::gameshookcallLLKB = lpfn;
		}
		else if (idHook == WH_MOUSE_LL)
		{
			SetWindowsHookHook::LLMousehooked = true;
			SetWindowsHookHook::gameshookcallLLMouse = lpfn;
		}
		else if (idHook == WH_GETMESSAGE)
		{
			SetWindowsHookHook::Messagehooked = true;
			SetWindowsHookHook::gameshookcallMessage = lpfn;
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

	void SetWindowsHookHook::FireFakeGetMessage(int message, WPARAM wParam, LPARAM lParam)
	{
		//MessageBoxA(NULL, "a", "s", MB_OK);
		MSG msg;
		msg.lParam = lParam;
		msg.message = message;
		msg.wParam = wParam;
		gameshookcallMessage(HC_ACTION, PM_REMOVE, (LPARAM)&msg); //or PM_NOREMOVE?
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
		if (SetWindowsHookHook::Messagehooked || SetWindowsHookHook::LLKBhooked || SetWindowsHookHook::LLMousehooked)
		{
			ImGui::Text("SetWindowsHookEx has been called by the game.");
			ImGui::Text("Callbacks listed below:");
			if (SetWindowsHookHook::Messagehooked)
				ImGui::Text("Message callback activated");
			if (SetWindowsHookHook::LLKBhooked)
				ImGui::Text("lowlevel Keyboard callback activated");
			if (SetWindowsHookHook::LLMousehooked)
				ImGui::Text("lowlevel mouse callback activated");
		}
		else
		{
			ImGui::Text("SetWindowsHookEx may not been called by the game.");
		}
	}

	void SetWindowsHookHook::InstallImpl()
	{
		hookInfoWndHookExA = std::get<1>(InstallNamedHook(L"user32", "SetWindowsHookExA", SetWindowsHookExAHook));
		hookInfoWndHookExW = std::get<1>(InstallNamedHook(L"user32", "SetWindowsHookExW", SetWindowsHookExWHook));
		hookInfoNextHookEx = std::get<1>(InstallNamedHook(L"user32", "CallNextHookEx", CallNextHookExHook));
	}
	
	void SetWindowsHookHook::UninstallImpl()
	{
		UninstallHook(&hookInfoWndHookExA);
	}

}
