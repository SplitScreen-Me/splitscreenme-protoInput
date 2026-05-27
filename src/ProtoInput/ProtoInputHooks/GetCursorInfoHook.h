#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class GetCursorInfoHook final : public Hook
	{
	private:
		HookInfo hookInfo{};

	public:
		const char* GetHookName() const override { return "GetCursorInfo"; }
		const char* GetHookDescription() const override { return "Hooks the GetCursorInfo function to return our 'fake' position"; }
		bool HasGuiStatus() const override { return false; }
		void ShowGuiStatus() override;
		void InstallImpl() override;
		void UninstallImpl() override;
	};

}
