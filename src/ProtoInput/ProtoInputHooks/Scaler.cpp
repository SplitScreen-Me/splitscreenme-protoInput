#include "Scaler.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "SetWindowPosHook.h"
#include <imgui.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


namespace Proto
{

    int scalewidth = 0;
    int scaleheight = 0;
    int origwidth = 0;
    int origheight = 0;

	bool Enableornot = false;
    bool onlyonetimethis = false;

    WNDPROC g_OldWndProc = nullptr;

    POINT Scaler::getfactor(POINT pp){
        if (pp.x != 0 && pp.y != 0 && Enableornot)
        { 
            float scalex = float(origwidth) / float(scalewidth);
            float scaley = float(origheight) / float(scaleheight);
            pp.x = static_cast<int>(std::lround(pp.x * scalex));
            pp.y = static_cast<int>(std::lround(pp.y * scaley));
        }
		return pp;
    }
    LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_MOUSEHOVER: // + MOVE // + click 
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);
            
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);
            break;
        }
        case WM_MOUSEMOVE:
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);

            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);

            break;
        }
        case WM_LBUTTONUP:
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);

            break;
        }
        case WM_RBUTTONDOWN:
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);

            break;
        }
        case WM_RBUTTONUP:
        {
            const auto& state = Proto::FakeMouseKeyboard::GetMouseState();
            POINT clientPos = { state.x, state.y };
            clientPos = Scaler::getfactor(clientPos);
            LPARAM newLParam = MAKELPARAM(clientPos.x, clientPos.y);
            return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, newLParam);

            break;
        }

        }
        // Forward all other messages unchanged
        return CallWindowProc(g_OldWndProc, hwnd, msg, wParam, lParam);
    }

	void Scaler::Install()
	{
       // MessageBoxA(NULL, "hihu", "letssee", MB_OK);
        if (Enableornot && !onlyonetimethis)
        { 
			//MessageBoxA(NULL, "Ohyea", "Scaler enabled", MB_OK);
        
            g_OldWndProc = (WNDPROC)SetWindowLongPtr(
                (HWND)Proto::HwndSelector::GetSelectedHwnd(),
                GWLP_WNDPROC,
                (LONG_PTR)SubclassProc
		    );
            onlyonetimethis = true; //or else crash
        }
	}
    void Scaler::Uninstall()
    {
        if (onlyonetimethis)
        {
            HWND hwnd = (HWND)Proto::HwndSelector::GetSelectedHwnd();

            SetWindowLongPtr(
                hwnd,
                GWLP_WNDPROC,
                (LONG_PTR)g_OldWndProc
            );

            onlyonetimethis = false;
        }
    }

    void Scaler::Settings(int oldX, int oldY, int newX, int newY)
    {
		Enableornot = true;
		scalewidth = newX;
	    scaleheight = newY;
        origwidth = oldX;
        origheight = oldY;
        if (scalewidth > 5 && scaleheight > 5 && origwidth > 5 && origheight > 5)
            Enableornot = true;
        else Enableornot = false;
        if (Enableornot)
            Scaler::Install();
        else Scaler::Uninstall();
    }
}
