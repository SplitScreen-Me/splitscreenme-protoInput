#pragma once
#include <windows.h>
#include <cstdint>

namespace Proto
{

extern unsigned long GuiThreadID;
extern intptr_t ConsoleHwnd;
extern HWND ProtoGuiHwnd;
extern bool DisableGuiWindow;

int ShowGuiImpl();

void RenderImgui();

void ToggleWindow();
void SetWindowVisible(bool visible);

void ToggleConsole();
void SetConsoleVisible(bool visible);

}
