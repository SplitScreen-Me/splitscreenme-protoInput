#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class SetWindowsHookHook final : public Hook
	{
	private:
		HookInfo hookInfoWndHookExA{};
		HookInfo hookInfoWndHookExW{};
		HookInfo hookInfoNextHookEx{};


	public:
		const char* GetHookName() const override { return "SetWindowsHook Hook"; }
		const char* GetHookDescription() const override
		{
			return
				"Hooks SetWindowsHook function to prevent windows from hooking and bypassing protoinput. "
				"Not many games calls this function, but if it does, then input cant be filtered. ";
		}

		static HOOKPROC gameshookcallLLKB;
		static HOOKPROC gameshookcallLLMouse;

		static HOOKPROC gameshookcallMessage;

		static tagKBDLLHOOKSTRUCT Kbstate;

		bool HasGuiStatus() const override { return true; }
		void ShowGuiStatus() override;

		static void FireFakeLLMouseMove(int x, int y);
		static void FireFakeLLKeyboardEvent(int vkey, bool pressed);
		static void FireFakeGetMessage(int message, WPARAM wParam, LPARAM lParam);

		static bool LLKBhooked;
		static bool LLMousehooked;

		static bool Messagehooked;

		void InstallImpl() override;
		void UninstallImpl() override;
	};

}