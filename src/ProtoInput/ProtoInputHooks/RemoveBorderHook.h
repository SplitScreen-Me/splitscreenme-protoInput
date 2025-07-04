#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class RemoveBorderHook final : public Hook
	{
	private:
		HookInfo hookInfoA{};
		HookInfo hookInfoW{};
		HookInfo hookInfoPtrA{};
		HookInfo hookInfoPtrW{};

	public:

		static bool DontWaitWindowBorder;

		const char* GetHookName() const override { return "Remove Border"; }
		const char* GetHookDescription() const override
		{
			return
				"Hooks SetWindowLong and SetWindowLongPtr. And maintain the window aspect after the border removal through EnumWindowsProc. ";
		}
		bool HasGuiStatus() const override { return true; }

		void InstallImpl() override;
		void UninstallImpl() override;
	};

}