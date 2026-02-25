#include "Gui.h"
#include "HwndSelector.h"
#include <cstdio>
#include "FakeCursor.h"
#include "TranslateXtoMKB.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")



namespace Proto
{

intptr_t HwndSelector::selectedHwnd = 0;
int HwndSelector::windowWidth, HwndSelector::windowHeight;

struct HandleData
{
    unsigned long pid;
    HWND hwnd;
};
int somonge = 0;
BOOL IsMainWindow(HWND hwnd)
{
    if (!IsWindow(hwnd)) return FALSE;

    // Must be visible
    if (!IsWindowVisible(hwnd)) return FALSE;

    // Must not be a child window
    if (GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD) return FALSE;

    RECT rect;
    int width; int height;
    if (GetClientRect(hwnd, &rect))
    {
        width = rect.right - rect.left;
    }
    if (width < 400)
    {
        //MessageBoxA(NULL, "neikasvarte. DWM bredder", "oja", MB_OK);
        HRESULT hr = DwmGetWindowAttribute(
            (HWND)hwnd,
            DWMWA_EXTENDED_FRAME_BOUNDS,
            &rect,
            sizeof(rect)
        );

        if (SUCCEEDED(hr)) {
            width = rect.right - rect.left;
        }

    }
    if (width < 400)
    {
        return FALSE;
    }

    // Exclude your own windows
    if (hwnd == (HWND)Proto::ConsoleHwnd) return FALSE;
    if (hwnd == Proto::ProtoGuiHwnd) return FALSE;
    if (hwnd == FakeCursor::GetPointerWindow()) return FALSE;

    if (GetWindow(hwnd, GW_OWNER) == (HWND)0)
       return TRUE;
    return FALSE;
}


BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    HandleData& data = *(HandleData*)lParam;
	
    DWORD pid = 0;
    GetWindowThreadProcessId(handle, &pid);
	
    if (data.pid != pid || !IsMainWindow(handle))
        return TRUE; // Keep searching
	
    data.hwnd = handle;
	
    return FALSE;
}

void HwndSelector::UpdateMainHwnd(bool logOutput)
{
    // Go through all the top level windows, select the first that's visible & belongs to the process
	
    HandleData data { GetCurrentProcessId(), nullptr };
    EnumWindows(EnumWindowsCallback, (LPARAM)&data);

    auto hwnd = (intptr_t)data.hwnd;
    
    if (logOutput)
    {



        if (hwnd == 0)
        {
            printf("Warning: UpdateMainHwnd didn't find a main window\n");

        }
        else //EnumChildWindows(parent, EnumChildProc, (LPARAM)pid);
            printf("UpdateMainHwnd found hwnd %d (0x%X)\n", hwnd, hwnd);
      //  else {
       //     MessageBoxA(NULL, "neikasvarte ingen bredde likevel. prøver childs", "oja", MB_OK);
		//	EnumChildWindows((HWND)hwnd, EnumWindowsCallback, (LPARAM)&data);
		//	hwnd = (intptr_t)data.hwnd;
		//	if (hwnd == 0)
		//	{
		//		printf("Warning: UpdateMainHwnd didn't find a main window even after searching children\n");
		//	}
		//	else
		//	{
       //         MessageBoxA(NULL, "oja, fant noe allikevel.", "oja", MB_OK);
		//		printf("UpdateMainHwnd found child hwnd %d (0x%X)\n", hwnd, hwnd);
		//	}
      // }
    }
		
    if (data.hwnd != nullptr)
        selectedHwnd = (intptr_t)data.hwnd;
}

void HwndSelector::UpdateWindowBounds()
{
    RECT rect;
    if (GetClientRect((HWND)selectedHwnd, &rect))
    {
        if ((rect.right - rect.left) > 5)
        { 
            windowWidth = rect.right - rect.left;
            windowHeight = rect.bottom - rect.top;
        }
        else {
            HRESULT hr = DwmGetWindowAttribute(
                (HWND)selectedHwnd,
                DWMWA_EXTENDED_FRAME_BOUNDS,
                &rect,
                sizeof(rect)
            );

            if (SUCCEEDED(hr)) {
                windowWidth = rect.right - rect.left;
                windowHeight = rect.bottom - rect.top;
            }

        }

    }
    else
        fprintf(stderr, "GetClientRect failed in update main window bounds\n");
    if (windowWidth < 400)
        windowWidth = 800;
    if (windowHeight < 400)
        windowHeight = 600;
}

void HwndSelector::SetSelectedHwnd(intptr_t set)
{
    selectedHwnd = set;
    UpdateWindowBounds();
}

}
