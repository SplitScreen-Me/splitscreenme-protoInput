#include "MoveWindowHook.h"
#include <imgui.h>

namespace Proto
{

	int MoveWindowHook::width = 0;
	int MoveWindowHook::height = 0;
	int MoveWindowHook::posx = 0;
	int MoveWindowHook::posy = 0;

	bool MoveWindowHook::MoveWindowDontResize = false;
	bool MoveWindowHook::MoveWindowDontReposition = false;

	BOOL WINAPI Hook_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
	{
		int Width = MoveWindowHook::MoveWindowDontResize ? nWidth : MoveWindowHook::width;
		int Height = MoveWindowHook::MoveWindowDontResize ? nHeight : MoveWindowHook::height;
		int PosX = MoveWindowHook::MoveWindowDontReposition ? X : MoveWindowHook::posx;
		int PosY = MoveWindowHook::MoveWindowDontReposition ? Y : MoveWindowHook::posy;

		return MoveWindow(hWnd, PosX, PosY, Width, Height, bRepaint);
	}

	void MoveWindowHook::ShowGuiStatus()
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

	void MoveWindowHook::InstallImpl()
	{
		hookInfo = std::get<1>(InstallNamedHook(L"user32", "MoveWindow", Hook_MoveWindow));
	}

	void MoveWindowHook::UninstallImpl()
	{
		UninstallHook(&hookInfo);
	}

}
