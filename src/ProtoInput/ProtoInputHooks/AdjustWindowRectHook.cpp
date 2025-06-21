#include "AdjustWindowRectHook.h"
#include <imgui.h>

namespace Proto
{

	int AdjustWindowRectHook::width = 0;
	int AdjustWindowRectHook::height = 0;
	int AdjustWindowRectHook::posx = 0;
	int AdjustWindowRectHook::posy = 0;

	BOOL WINAPI Hook_AdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
	{
		lpRect->top = AdjustWindowRectHook::posy;
		lpRect->left = AdjustWindowRectHook::posx;
		lpRect->bottom = AdjustWindowRectHook::height + AdjustWindowRectHook::posy;
		lpRect->right = AdjustWindowRectHook::width + AdjustWindowRectHook::posx;

		return AdjustWindowRect(lpRect, dwStyle, false);;
	}

	BOOL WINAPI Hook_AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
	{
		lpRect->top = AdjustWindowRectHook::posy;
		lpRect->left = AdjustWindowRectHook::posx;
		lpRect->bottom = AdjustWindowRectHook::height + AdjustWindowRectHook::posy;
		lpRect->right = AdjustWindowRectHook::width + AdjustWindowRectHook::posx;

		return AdjustWindowRectEx(lpRect, dwStyle, false, dwExStyle);;
	}

	void AdjustWindowRectHook::ShowGuiStatus()
	{
		int pos[2] = { posx, posy };
		ImGui::SliderInt2("Position", &pos[0], -5000, 5000);
		posx = pos[0];
		posy = pos[1];

		int size[2] = { width, height };
		ImGui::SliderInt2("Size", &size[0], 0, 5000);
		width = size[0];
		height = size[1];
	}

	void AdjustWindowRectHook::InstallImpl()
	{
		hookInfoRect = std::get<1>(InstallNamedHook(L"user32", "AdjustWindowRect", Hook_AdjustWindowRect));
		hookInfoRectEx = std::get<1>(InstallNamedHook(L"user32", "AdjustWindowRectEx", Hook_AdjustWindowRectEx));
	}

	void AdjustWindowRectHook::UninstallImpl()
	{
		UninstallHook(&hookInfoRect);
		UninstallHook(&hookInfoRectEx);
	}

}
