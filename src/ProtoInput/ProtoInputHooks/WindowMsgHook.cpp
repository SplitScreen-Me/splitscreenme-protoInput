#include "WindowMsgHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "SetWindowPosHook.h"
#include <imgui.h>
#include <dwmapi.h>
#include "windowsx.h"
#pragma comment(lib, "dwmapi.lib")


namespace Proto
{
    int scalewidth = 0;
    int scaleheight = 0;
    int origwidth = 0;
    int origheight = 0;

	bool Enableornot = false;
    bool onlyonetimethis = false;
	bool scalenotpointer = false;

    WNDPROC g_OldWndProc = nullptr;

    WPARAM defaultmove = 0x00060001;
    WPARAM primary = 0x00100000;
    WPARAM secondary = 0x00200000;
    WPARAM third = 0x00400000;
    WPARAM fourth = 0x00800000;
    WPARAM fifth = 0x01000000;

    int now = 0;
    bool leftdown, rightdown = false;
    bool tredown, firdown, femdown = false;

    void UninstallWndProc()
    {
        if (onlyonetimethis)
        {
            HWND hwnd = (HWND)Proto::HwndSelector::GetSelectedHwnd();
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_OldWndProc);
            onlyonetimethis = false;
        }
    }

    void WindowMsgHook::Uninstall()
    {
        UninstallWndProc();
    }

    WPARAM updatePointer(bool pressed, WPARAM button)
    {
        WPARAM returned;

        returned = button;

        if (returned == primary)
        {
            // wParam = defaultmove;
            if (rightdown) returned += secondary;
            if (tredown) returned += third;
            if (firdown) returned += fourth;
            if (femdown) returned += fifth;
        }
        else if (returned == secondary)
        {
            if (leftdown) returned += primary;
            if (tredown) returned += third;
            if (firdown) returned += fourth;
            if (femdown) returned += fifth;
        }
        else if (returned == third)
        {
            if (leftdown) returned += primary;
            if (rightdown) returned += secondary;
            if (firdown) returned += fourth;
            if (femdown) returned += fifth;
        }
        else if (returned == fourth)
        {
            if (leftdown) returned += primary;
            if (rightdown) returned += secondary;
            if (tredown) returned += third;
            if (femdown) returned += fifth;
        }
        else if (returned == fifth)
        {
            if (leftdown) returned += primary;
            if (rightdown) returned += secondary;
            if (tredown) returned += third;
            if (firdown) returned += fourth;
        }
        if (pressed)
            returned += defaultmove;
        else
            returned += 0x00020001;

        return returned;
    }

    POINT WindowMsgHook::getfactor(POINT pp)
    {
        if (pp.x != 0 && pp.y != 0 && Enableornot)
        { 
            float scalex = float(origwidth) / float(scalewidth);
            float scaley = float(origheight) / float(scaleheight);
            pp.x = static_cast<int>(std::lround(pp.x * scalex));
            pp.y = static_cast<int>(std::lround(pp.y * scaley));
        }
		return pp;
    }

    LPARAM GetScaledLParam(HWND hwnd, bool clienttoscreen) 
    {
        const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
        POINT clientPos = { state.x, state.y };
        clientPos = WindowMsgHook::getfactor(clientPos);
        if (clienttoscreen)
            ClientToScreen(hwnd, &clientPos);
        return MAKELPARAM(clientPos.x, clientPos.y);
	}

    LPARAM ProcessedLparam(LPARAM lParam, HWND hwnd, bool clienttoscreen) 
    {
        POINT clientPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (clienttoscreen)
            ClientToScreen(hwnd, &clientPos);
        return MAKELPARAM(clientPos.x, clientPos.y);
    }

    LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {

        case WM_MOUSEMOVE:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));

            msg = WM_POINTERUPDATE;

            if (!leftdown && !rightdown && !tredown && !firdown && !femdown)
                wParam = 0x20020001;
            else 
            {
                wParam = defaultmove;
                if (leftdown) wParam += primary;
                if (rightdown) wParam += secondary;
                if (tredown) wParam += third;
                if (firdown) wParam += fourth;
                if (femdown) wParam += fifth;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_LBUTTONDOWN:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));

            leftdown = true;
            if (!rightdown && !tredown && !firdown && !femdown)
            {
                wParam = 0x20160001;
                msg = WM_POINTERDOWN;
            }
            else {
                wParam = updatePointer(true, primary);
                msg = WM_POINTERUPDATE;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_LBUTTONUP:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            leftdown = false;
            if (!rightdown && !tredown && !firdown && !femdown)
            {
                wParam = 0x00020001;
                msg = WM_POINTERUP;
            }
            else {
                wParam = updatePointer(false, primary);
                msg = WM_POINTERUPDATE;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_RBUTTONDOWN:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            rightdown = true;
            if (!leftdown && !tredown && !firdown && !femdown)
            {
                wParam = 0x20260001;
                msg = WM_POINTERDOWN;
            }
            else {
                wParam = updatePointer(true, secondary);
                msg = WM_POINTERUPDATE;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_RBUTTONUP:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            rightdown = false;
            if (!leftdown && !tredown && !firdown && !femdown)
            {
                wParam = 0x00020001;
                msg = WM_POINTERUP;
            }
            else {
                wParam = updatePointer(false, secondary);
                msg = WM_POINTERUPDATE;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_MBUTTONDOWN:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            tredown = true;
            if (!rightdown && !leftdown && !firdown && !femdown)
            {
                wParam = 0x20460001;
                msg = WM_POINTERDOWN;
            }
            else {
                wParam = updatePointer(true, third);
                msg = WM_POINTERUPDATE;
            }
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_MBUTTONUP:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            tredown = false;
            if (!rightdown && !leftdown && !firdown && !femdown)
            {
                wParam = 0x00020001;
                msg = WM_POINTERUP;
            }
            else {
                wParam = updatePointer(false, third);
                msg = WM_POINTERUPDATE;
            }

            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }

        case WM_XBUTTONDOWN:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            int fwButton = GET_XBUTTON_WPARAM(wParam);
            if (fwButton == XBUTTON1)
            {
                firdown = true;
                if (!rightdown && !leftdown && !tredown && !femdown)
                {
                    wParam = 0x20860001;
                    msg = WM_POINTERDOWN;
                }
                else
                {
                    wParam = updatePointer(true, fourth);
                    msg = WM_POINTERUPDATE;
                }
            }
            else if (fwButton == XBUTTON2)
            {
                if (scalenotpointer)
                    return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
                femdown = true;
                if (!rightdown && !leftdown && !tredown && !firdown)
                {
                    wParam = 0x21060001;
                    msg = WM_POINTERDOWN;
                }
                else {
                    wParam = updatePointer(true, fifth);
                    msg = WM_POINTERUPDATE;
                }
            }
            else return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam); //fwbutton unrecongnized

            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_XBUTTONUP:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, false));
            int fwButton = GET_XBUTTON_WPARAM(wParam);
            if (fwButton == XBUTTON1)
            {
                firdown = false;
                if (!rightdown && !leftdown && !tredown && !femdown)
                {
                    msg = WM_POINTERUP;
                    wParam = 0x00020001;
                }
                else {
                    wParam = updatePointer(false, fourth);
                    msg = WM_POINTERUPDATE;
                }
            }
            else if (fwButton == XBUTTON2)
            {
                femdown = false;
                if (!rightdown && !leftdown && !tredown && !firdown)
                {
                    msg = WM_POINTERUP;
                    wParam = 0x00020001;
                }
                else {
                    wParam = updatePointer(false, fifth);
                    msg = WM_POINTERUPDATE;
                }
            }
            else return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam); //none of these buttons?

            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, ProcessedLparam(lParam, hwnd, true));
        }
        case WM_MOUSEWHEEL:
        {
            if (scalenotpointer)
                return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, GetScaledLParam(hwnd, true)); //client coordinates

            msg = WM_POINTERWHEEL;

			return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam); //lParam should be screen coordinates already and that is correct
        }

        //less cpu usage blocking these here i guess as the wndproc is already hooked
		case WM_POINTERUPDATE: 
		case WM_POINTERDOWN: 
        case WM_POINTERLEAVE: 
        case WM_POINTERCAPTURECHANGED: //POINTERCAPTURECHANGED is not listed in proto yet
        case WM_MOUSELEAVE:
        {
            return 0;
        }
        case WM_QUIT:
        {
            UninstallWndProc();
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam);
        }
    }//end of switch
    return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam);
    }

	void WindowMsgHook::Install()
	{
        if (Enableornot && !onlyonetimethis)
        { 
            g_OldWndProc = (WNDPROC)SetWindowLongPtr(
                (HWND)Proto::HwndSelector::GetSelectedHwnd(),
                GWLP_WNDPROC,
                (LONG_PTR)SubclassProc
		    );
            onlyonetimethis = true; //or else crash
        }
	}


    void WindowMsgHook::Settings(int oldX, int oldY, int newX, int newY)
    {
		Enableornot = true;
        
		scalewidth = newX;
	    scaleheight = newY;
        origwidth = oldX;
        origheight = oldY;

        //validating settings
        if (scalewidth > 5 && scaleheight > 5 && origwidth > 5 && origheight > 5)
        { 
            scalenotpointer = true;
            Enableornot = true;
        } 
        else Enableornot = false;
        if (Enableornot)
            WindowMsgHook::Install();
    }
    void WindowMsgHook::PointerInMouse( bool enable)
    {

        scalenotpointer = false;
        Enableornot = enable;
        if (enable)
            WindowMsgHook::Install();
        else WindowMsgHook::Uninstall();
    }
}
