#include "Gui.h"
#include "HwndSelector.h"
#include <cstdio>
#include "FakeCursor.h"
#include "INISettings.h"


namespace Proto
{

intptr_t HwndSelector::selectedHwnd = 0;
int HwndSelector::windowWidth, HwndSelector::windowHeight;

struct HandleData
{
    unsigned long pid;
    HWND hwnd;
    const char* winName;
    const char* winClass;
};

BOOL IsMainWindow(HWND handle)
{
	// Is top level & visible & not one of ours
    return
		GetWindow(handle, GW_OWNER) == (HWND)0 && 
        IsWindowVisible(handle) &&
        handle != (HWND)Proto::ConsoleHwnd &&
        handle != Proto::ProtoGuiHwnd &&
        handle != FakeCursor::GetPointerWindow();
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    HandleData& data = *(HandleData*)lParam;
	
    DWORD pid = 0;
    GetWindowThreadProcessId(handle, &pid);
	
    if (data.pid != pid)
        return TRUE; // Keep searching
	
    bool strictMode = (data.winName && data.winName[0] != '\0') ||
        (data.winClass && data.winClass[0] != '\0');

    if (strictMode)
    {
        if (data.winClass && data.winClass[0] != '\0') {
            char className[256];
            GetClassNameA(handle, className, sizeof(className));
            if (strcmp(className, data.winClass) != 0) return TRUE;
        }

        if (data.winName && data.winName[0] != '\0') {
            char windowName[256];
            GetWindowTextA(handle, windowName, sizeof(windowName));
            if (strcmp(windowName, data.winName) != 0) return TRUE;
        }

        if (!IsWindowVisible(handle)) return TRUE;
    }
    else
    {
        if (!IsMainWindow(handle)) return TRUE;
    }

    data.hwnd = handle;

    return FALSE;
}

void HwndSelector::UpdateMainHwnd(bool logOutput)
{
    // If not custom window, go through all the top level windows, select the first that's visible & belongs to the process
	
    HandleData data = { GetCurrentProcessId(), nullptr, customWindowName, customClassName };
    EnumWindows(EnumWindowsCallback, (LPARAM)&data);

    const auto hwnd = (intptr_t)data.hwnd;

    if (logOutput)
    {
        if (hwnd == 0)
            printf("Warning: UpdateMainHwnd didn't find a main window\n");
        else
            printf("UpdateMainHwnd found hwnd %d (0x%X)\n", hwnd, hwnd);
    }
		
    if (data.hwnd != nullptr)
            selectedHwnd = (intptr_t)data.hwnd;
}

void HwndSelector::UpdateWindowBounds()
{
    RECT rect;
    if (GetClientRect((HWND)selectedHwnd, &rect))
    {
        windowWidth = rect.right - rect.left;
        windowHeight = rect.bottom - rect.top;
    }
    else
        fprintf(stderr, "GetClientRect failed in update main window bounds\n");
}

void HwndSelector::SetSelectedHwnd(intptr_t set)
{
    selectedHwnd = set;
    UpdateWindowBounds();
}

}
