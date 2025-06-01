#include "SetWindowPosHook.h"
#include <imgui.h>

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
	ImGui::SliderInt2("Position", &pos[0], -5000, 5000);
	posx = pos[0];
	posy = pos[1];
	
	int size[2] = { width, height };
	ImGui::SliderInt2("Size", &size[0], 0, 5000);
	width = size[0];
	height = size[1];
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
