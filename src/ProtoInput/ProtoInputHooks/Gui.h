#pragma once
#include <windows.h>
#include <cstdint>

namespace Proto
{

extern unsigned long GuiThreadID;
extern intptr_t ConsoleHwnd;
extern HWND ProtoGuiHwnd;
extern HMODULE hmodule;


void Start();
int ShowGuiImpl();

void RenderImgui();
void StartGUIThread();
void ToggleWindow();
void SetWindowVisible(bool visible);

void ToggleConsole();
void SetConsoleVisible(bool visible);

}
