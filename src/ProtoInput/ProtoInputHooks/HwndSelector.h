#pragma once
#include <cstdint>
#include <vector>
#include <Windows.h>
namespace Proto
{

class HwndSelector
{
	static intptr_t selectedHwnd;
	
public:
	static int windowWidth, windowHeight;
	static std::vector<HWND> allwindows;
	//static std::vector<RECT> allwindowsrect;
	static intptr_t& GetSelectedHwnd() { return selectedHwnd; }
	static intptr_t const * GetSelectedHwndPtr() { return &selectedHwnd; }
	static void SetSelectedHwnd(intptr_t set);
	static void UpdateMainHwnd(bool logOutput = true);
	static void UpdateWindowBounds();
	static bool RemoteHwndEnabled;
	static void GetAllProcessWindows();
};

}
