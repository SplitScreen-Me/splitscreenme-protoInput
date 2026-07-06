#include "Gui.h"
#include "HwndSelector.h"
#include <cstdio>
#include "FakeCursor.h"
#include "INISettings.h"

#include "shellscalingapi.h"

#pragma comment(lib, "Shcore.lib") //for scaling
namespace Proto
{

    std::vector<HWND> HwndSelector::allwindows;
    //std::vector<RECT> HwndSelector::allwindowsrect;
    intptr_t HwndSelector::selectedHwnd = 0;
    int HwndSelector::windowWidth, HwndSelector::windowHeight;
    bool HwndSelector::RemoteHwndEnabled = false;

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

    //for available windows list in runtime gui
    BOOL CALLBACK EnumChildWindowsCallback(HWND hwnd, LPARAM lParam)
    {
        HwndSelector::allwindows.push_back(hwnd);
        return TRUE;
    }
    //for available windows list in runtime gui
    BOOL CALLBACK EnumWindowsCallbacklistall(HWND hwnd, LPARAM lParam) //children too
    {
        HandleData& data = *(HandleData*)lParam;
        DWORD windowPid = 0;
        GetWindowThreadProcessId(hwnd, &windowPid);
        if (data.pid == windowPid && IsMainWindow(hwnd))
        {
            HwndSelector::allwindows.push_back(hwnd);
            EnumChildWindows(hwnd, EnumChildWindowsCallback, 0);
        }
        return TRUE;
    }
    //for available windows list in runtime gui
    void HwndSelector::GetAllProcessWindows()
    {
        HwndSelector::allwindows.clear();
        HandleData data{ GetCurrentProcessId(), nullptr };
        EnumWindows(EnumWindowsCallbacklistall, (LPARAM)&data);
        return;
    }

    void HwndSelector::UpdateMainHwnd(bool logOutput)
    {
    // Check if remote hwnd is enabled
		if (HwndSelector::RemoteHwndEnabled)
		{
			if (logOutput)
				printf("Remote hwnd enabled, skipping search for main window\n");
			return;
		}

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

    int factor = 0;
    int oldfactor = 0;
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

        HMONITOR monitor;
        monitor = MonitorFromWindow(
            (HWND)selectedHwnd,
            MONITOR_DEFAULTTONEAREST
        );
        
        UINT dpiX, dpiY;
        GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        factor = (dpiX * 100) / 96;
        if (factor != oldfactor)
        {
			FakeCursor::setmonitorscale((int)factor);
			oldfactor = factor;
        }
    }

    void HwndSelector::SetSelectedHwnd(intptr_t set)
    {
        selectedHwnd = set;
        UpdateWindowBounds();
    }

}