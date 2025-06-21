#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class AdjustWindowRectHook final : public Hook
	{
	private:
		HookInfo hookInfoRect{};
		HookInfo hookInfoRectEx{};

	public:
		static int width;
		static int height;
		static int posx;
		static int posy;

		const char* GetHookName() const override { return "Adjust Window Rectangle"; }
		const char* GetHookDescription() const override
		{
			return
				"Hooks AdjustWindowRect and AdjustWindowRectEx. And calculates the required size of the window rectangle. ";
		}
		bool HasGuiStatus() const override { return true; }
		void ShowGuiStatus() override;
		void InstallImpl() override;
		void UninstallImpl() override;
	};

}
