#include "SetWindowPosHook.h"
#include <imgui.h>
#include "HwndSelector.h"

namespace Proto
{

int SetWindowPosHook::width = 0;
int SetWindowPosHook::height = 0;
int SetWindowPosHook::posx = 0;
int SetWindowPosHook::posy = 0;

bool SetWindowPosHook::SetWindowPosDontResize = false;
bool SetWindowPosHook::SetWindowPosDontReposition = false;

BOOL WINAPI Hook_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	int Width = SetWindowPosHook::SetWindowPosDontResize ? cx : SetWindowPosHook::width;
	int Height = SetWindowPosHook::SetWindowPosDontResize ? cy : SetWindowPosHook::height;
	int PosX = SetWindowPosHook::SetWindowPosDontReposition ? X : SetWindowPosHook::posx;
	int PosY = SetWindowPosHook::SetWindowPosDontReposition ? Y : SetWindowPosHook::posy;

	return SetWindowPos(hWnd, hWndInsertAfter, PosX, PosY, Width, Height, uFlags);
}

void SetWindowPosHook::ShowGuiStatus()
{
	int pos[2] = { posx, posy };
	ImGui::InputInt2("Position", &pos[0]);
	posx = pos[0];
	posy = pos[1];
	
	int size[2] = { width, height };
	ImGui::InputInt2("Size", &size[0]);
	width = size[0];
	height = size[1];

	ImGui::Separator();

	if (ImGui::Button("Apply Changes"))
	{
		if ((HWND)HwndSelector::GetSelectedHwnd() != NULL)
		{
			::SetWindowPos((HWND)HwndSelector::GetSelectedHwnd(), NULL, posx, posy, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

void SetWindowPosHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "SetWindowPos", Hook_SetWindowPos));
}

void SetWindowPosHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
