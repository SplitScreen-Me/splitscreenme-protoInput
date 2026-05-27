#include <OpenXinput.h>
#include "Gui.h"
#include <string>
#include <map>
#include <unordered_map>
#include "TranslateXtoMKB.h"
#include "RawInput.h"
#include <imgui.h>
#include <cmath>
#define NOMINMAX
#include <algorithm>
#include "EasyHook.h"
#include <vector>
#include <thread>
#include "HwndSelector.h"
#include "FakeCursor.h"
#include "Gui.h"
#include "GtoMnK_RawInput.h"
#include "FakeMouseKeyboard.h"
#include "ScanThread.h"
#include "StateInfo.h"


namespace ScreenshotInput
{
    int InstanceID = 0; //InstanceID copy from stateinfo.h

    int TranslateXtoMKB::RefreshWindow;
    int TranslateXtoMKB::RefreshPoint;

    //from tunnell
    int TranslateXtoMKB::controllerID;
    bool TranslateXtoMKB::rawinputhook; //registerrawinputhook
    bool TranslateXtoMKB::registerrawinputhook; //registerrawinputhook
    int TranslateXtoMKB::stickrightmapping;
    int TranslateXtoMKB::stickleftmapping;
    int TranslateXtoMKB::stickupmapping;
    int TranslateXtoMKB::stickdownmapping;
    int TranslateXtoMKB::Amapping;
    int TranslateXtoMKB::Bmapping;
    int TranslateXtoMKB::Xmapping;
    int TranslateXtoMKB::Ymapping;
    int TranslateXtoMKB::RSmapping;
    int TranslateXtoMKB::LSmapping;
    int TranslateXtoMKB::rightmapping;
    int TranslateXtoMKB::leftmapping;
    int TranslateXtoMKB::upmapping;
    int TranslateXtoMKB::downmapping;
    int TranslateXtoMKB::stickRpressmapping;
    int TranslateXtoMKB::stickLpressmapping;
    int TranslateXtoMKB::optionmapping;
    int TranslateXtoMKB::startmapping;
    bool TranslateXtoMKB::lefthanded;
    int TranslateXtoMKB::mode = 1;
	bool TranslateXtoMKB::SaveBmps;

    int TranslateXtoMKB::Sens = 12;
    int TranslateXtoMKB::Sensmult = 4;
    int TranslateXtoMKB::Deadzone = 2;

    int updatewindowtick = 300;

    float axial_deadzone = 0.00f; // Square/Axial Deadzone (0.0 to 0.3)
    const float max_threshold = 0.03f; // Max Input Threshold, an "outer deadzone" (0.0 to 0.15)
    const float curve_slope = 0.16f; // The linear portion of the response curve (0.0 to 1.0)
    const float curve_exponent = 5.00f; // The exponential portion of the curve (1.0 to 10.0)

    int startbuttontimer, backbuttontimer;


    HMODULE g_hModule = nullptr;

    bool loop = true;
    //int TranslateXtoMKB::showmessage = 0; //0 = no message, 1 = initializing, 2 = bmp mode, 3 = bmp and cursor mode, 4 = edit mode   

    int counter = 0;
    bool oldhome = false; //toggle lock input with home key

    //copy of criticals
    int modeMT;

    POINT delta;
    //hooks


    //fake cursor
    int Xf = 0;
    int Yf = 0;


    int tick = 0;

    bool leftPressedold = false;
    bool rightPressedold = false;
    bool oldA = false;
    bool oldB = false;
    bool oldX = false;
    bool oldY = false;

    bool oldC = false;
    bool oldD = false;
    bool oldE = false;
    bool oldF = false;

    bool olddown = false;
    bool oldup = false;
    bool oldleft = false;
    bool oldright = false;
    bool oldoptions = false;
    bool oldstart = false;
    bool oldstartoptions = false;
    bool oldscrolldown = false;
    bool oldscrollleft = false;
    bool oldscrollright = false;
    bool oldscrollup = false;
    bool oldGUIkey = false;


    void StartScanner() {
        ScanThread::numphotoA = 0;
        ScanThread::numphotoB = 0;
        ScanThread::numphotoX = 0;
        ScanThread::numphotoY = 0;
        // InitializeCriticalSection(&ScanThread::critical);
        if (!ScanThread::enumeratebmps()) //false means no bmps found. also counts statics
        {
            //  printf("BMPs enumerated but not found");
            if (ScanThread::scanoption)
            {
                ScanThread::scanoption = false;
                //    printf("Error. Nothing to scan for. Disabling scanoption");
            }
			
        }
        else {
            //  printf("BMPs found");
            ScanThread::staticPointA.assign(ScanThread::numphotoA + 1, POINT{ 0, 0 });
            ScanThread::staticPointB.assign(ScanThread::numphotoB + 1, POINT{ 0, 0 });
            ScanThread::staticPointX.assign(ScanThread::numphotoX + 1, POINT{ 0, 0 });
            ScanThread::staticPointY.assign(ScanThread::numphotoY + 1, POINT{ 0, 0 });
        }
        ScanThread::initovector();
        if (ScanThread::scanoption)
        { //starting bmp continous scanner
            ScanThread::StartScanThread(g_hModule, ScanThread::Aisstatic, ScanThread::Bisstatic, ScanThread::Xisstatic, ScanThread::Yisstatic, ScanThread::scanoption);
            //  printf("BMP scanner started");
        }
        Sleep(20); //give time for ScanThread::scanloop toggle true
    }

    USHORT lastVKkey = 0;
    bool IsKeyPressed(int Vkey)
    {
        return (GetAsyncKeyState(Vkey) & 0x8000) != 0;

    }
    void TranslateXtoMKB::SendMouseClick(int x, int y, int z) {
        // Create a named mutex
        RAWMOUSE muusjn = { 0 };
        muusjn.usButtonFlags = 0;

        muusjn.lLastX = x;
        muusjn.lLastY = y;

        if (z == 3) {
            muusjn.usButtonFlags |= RI_MOUSE_LEFT_BUTTON_DOWN;
            RawInput::GenerateRawMouseButton(-1, true);
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_LBUTTON, true); //last originally
        }
        if (z == 4)
        {
            muusjn.usButtonFlags |= RI_MOUSE_LEFT_BUTTON_UP;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_LBUTTON, false);
            RawInput::GenerateRawMouseButton(-1, false);
        }
        if (z == 5) {
            muusjn.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_DOWN;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_RBUTTON, true);
            RawInput::GenerateRawMouseButton(-2, true);
        }
        if (z == 6)
        {
            muusjn.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_UP;
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(VK_RBUTTON, false);
            RawInput::GenerateRawMouseButton(-2, false);

        }
        if (z == 20 || z == 21) //WM_mousewheel need desktop coordinates
        {
        }
        else if (z == 8 || z == 10 || z == 11) //only mousemove
        {
            RawInput::SendActionDelta(x, y);
            Proto::FakeMouseKeyboard::AddMouseDelta(x, y);

        }
        Proto::RawInput::SendInputMessages(muusjn);
    }
    std::string UGetExeFolder() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        std::string exePath(path);

        size_t lastSlash = exePath.find_last_of("\\/");
        return exePath.substr(0, lastSlash);
    }
    void ButtonStateImpulse(int vk, bool state, int whocalled)
    {
        if (modeMT == 1)
        {
            RAWKEYBOARD data{};
            data.MakeCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
            data.VKey = vk;
            data.ExtraInformation = 0;
            data.Flags = state ? 0 : RI_KEY_BREAK;
            data.Message = state ? WM_KEYDOWN : WM_KEYUP;
            // Extended key?
            switch (vk)
            {
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:
                case VK_INSERT:
                case VK_DELETE:
                case VK_HOME:
                case VK_END:
                case VK_PRIOR:
                case VK_NEXT:
                case VK_RCONTROL:
                case VK_RMENU:
                case VK_DIVIDE:
                case VK_NUMLOCK:
                    data.Flags |= RI_KEY_E0;
                    break;
            }
            Proto::FakeMouseKeyboard::ReceivedKeyPressOrRelease(vk, state);
            Proto::RawInput::SendKeyMessage(data, state);
            RawInput::GenerateRawKey(vk, state, false);
        }
        if (modeMT == 2)
        {
            if (Proto::FakeCursor::Showmessage == 0)
             {
				HWND hwndhere = (HWND)Proto::HwndSelector::GetSelectedHwnd();
                Proto::FakeMouseState muusjn = Proto::FakeMouseKeyboard::GetMouseState();
                int Xhold = muusjn.x;
                int Yhold = muusjn.y;
                std::string path;
                if ( whocalled == 0) //A
                { 
                    path = UGetExeFolder() + "\\A" + std::to_string(ScanThread::numphotoA) + ".bmp";
                    Proto::FakeCursor::Showmessage = 10; //signal is saving
                    ScanThread::numphotoA++;
                }
                if (whocalled == 1) //B
                {
                    path = UGetExeFolder() + "\\B" + std::to_string(ScanThread::numphotoB) + ".bmp";
                    Proto::FakeCursor::Showmessage = 11; //signal is saving
                    ScanThread::numphotoB++;
                }
                if (whocalled == 2) //X
                {
                    path = UGetExeFolder() + "\\X" + std::to_string(ScanThread::numphotoX) + ".bmp";
                    Proto::FakeCursor::Showmessage = 12; //signal is saving
                    ScanThread::numphotoX++;
                }
                if (whocalled == 3) //Y
                {
                    path = UGetExeFolder() + "\\Y" + std::to_string(ScanThread::numphotoY) + ".bmp";
                    Proto::FakeCursor::Showmessage = 13; //signal is saving
                    ScanThread::numphotoY++;
                }
				
                std::wstring wpath(path.begin(), path.end());
                ScanThread::SaveWindow10x10BMP(hwndhere, wpath.c_str(), muusjn.x, muusjn.y);
               // MessageBoxW(nullptr, wpath.c_str(), L"Path", MB_OK);

                TranslateXtoMKB::RefreshPoint ++;
                counter = 0;
              }
        }
    }



    std::wstring WGetExeFolder() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::wstring exePath(path);
        size_t lastSlash = exePath.find_last_of(L"\\/");

        if (lastSlash == std::wstring::npos)
            return L"";
        return exePath.substr(0, lastSlash);
    }


    bool IsTriggerPressed(BYTE triggerValue) {
        BYTE threshold = 175;
        return triggerValue > threshold;
    }


    // Helper: Get stick magnitude
    float GetStickMagnitude(SHORT x, SHORT y) {
        return sqrtf(static_cast<float>(x) * x + static_cast<float>(y) * y);
    }

    // Helper: Clamp value to range [-1, 1]
    float Clamp(float v) {
        if (v < -1.0f) return -1.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }

    POINT CalculateUltimateCursorMove(
        SHORT stickX, SHORT stickY,
        float c_deadzone,
        float s_deadzone,
        float max_threshold,
        float curve_slope,
        float curve_exponent,
        float sensitivity,
        float accel_multiplier
    ) {
        static double mouseDeltaAccumulatorX = 0.0;
        static double mouseDeltaAccumulatorY = 0.0;

        double normX = static_cast<double>(stickX) / 32767.0;
        double normY = static_cast<double>(stickY) / 32767.0;

        double magnitude = std::sqrt(normX * normX + normY * normY);
        if (magnitude < c_deadzone) {
            return { 0, 0 }; // Inside circular deadzone
        }
        if (std::abs(normX) < s_deadzone) {
            normX = 0.0; // Inside axial deadzone for X
        }
        if (std::abs(normY) < s_deadzone) {
            normY = 0.0; // Inside axial deadzone for Y
        }
        magnitude = std::sqrt(normX * normX + normY * normY);
        if (magnitude < 1e-6) {
            return { 0, 0 };
        }

        double effectiveRange = 1.0 - max_threshold - c_deadzone;
        if (effectiveRange < 1e-6) effectiveRange = 1.0;

        double remappedMagnitude = (magnitude - c_deadzone) / effectiveRange;
        remappedMagnitude = (std::max)(0.0, (std::min)(1.0, remappedMagnitude));

        double curvedMagnitude = curve_slope * remappedMagnitude + (1.0 - curve_slope) * std::pow(remappedMagnitude, curve_exponent);

        double finalSpeed = sensitivity * accel_multiplier;

        double dirX = normX / magnitude;
        double dirY = normY / magnitude;
        double finalMouseDeltaX = dirX * curvedMagnitude * finalSpeed;
        double finalMouseDeltaY = dirY * curvedMagnitude * finalSpeed;

        mouseDeltaAccumulatorX += finalMouseDeltaX;
        mouseDeltaAccumulatorY += finalMouseDeltaY;
        LONG integerDeltaX = static_cast<LONG>(mouseDeltaAccumulatorX);
        LONG integerDeltaY = static_cast<LONG>(mouseDeltaAccumulatorY);

        mouseDeltaAccumulatorX -= integerDeltaX;
        mouseDeltaAccumulatorY -= integerDeltaY;

        return { integerDeltaX, -integerDeltaY };
    }

    void TranslateXtoMKB::ThreadFunction()
    {
        if (ScanThread::scanoption && ScanThread::scanloop == false)
        {
			//MessageBoxA(NULL, "Starting BMP scanner...", "Info", MB_OK);
			StartScanner();
        }
        float sensitivity = static_cast<float>(TranslateXtoMKB::Sens);
        float accel_multiplier = static_cast<float>(TranslateXtoMKB::Sensmult) / 2.0f;
        float radial_deadzone = static_cast<float>(TranslateXtoMKB::Deadzone) / 20.0f;
		int deadzonelimit = 7000 + (TranslateXtoMKB::Deadzone * 1300);
        EnterCriticalSection(&ScanThread::critical);
        if (ScanThread::UpdateWindow)
            TranslateXtoMKB::RefreshPoint = 1;
        LeaveCriticalSection(&ScanThread::critical);
        modeMT = 1;
        if (TranslateXtoMKB::SaveBmps) {
            modeMT = 2;
        }
            
        //GUI
        if (oldGUIkey)
        {
            if (IsKeyPressed(VK_RCONTROL) && IsKeyPressed(VK_RMENU) && IsKeyPressed(0x30 + InstanceID))
            {
            }
            else
            {
                oldGUIkey = false;
                ButtonStateImpulse(VK_HOME, false, 99);//down}
            }
        }
        else if (IsKeyPressed(VK_RCONTROL) && IsKeyPressed(VK_RMENU) && IsKeyPressed(0x30 + InstanceID))//gui or fake cursor toggle
        {
            Proto::ToggleWindow();
            oldGUIkey = true;
            Proto::FakeCursor::Showmessage = 2;
            TranslateXtoMKB::RefreshPoint = 1;
            tick = 0;
            if (TranslateXtoMKB::mode == 1) //modechange
            {
                TranslateXtoMKB::mode = 2;
            }
            else TranslateXtoMKB::mode = 1;
        }

        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        // Check controller 0
        DWORD dwResult = OpenXInputGetState(TranslateXtoMKB::controllerID, &state);
        if (dwResult == ERROR_SUCCESS)
        {
            WORD buttons = state.Gamepad.wButtons;
            if (Proto::FakeCursor::Showmessage == 1)
            {//remove disconnected message
                Proto::FakeCursor::Showmessage = 0;
                TranslateXtoMKB::RefreshPoint = 1;
            }
            if (modeMT > 0)
            {
                //fake cursor poll
                int Xaxis = 0;
                int Yaxis = 0;
                int scrollXaxis = 0;
                int scrollYaxis = 0;
                int Yscroll = 0;
                int Xscroll = 0;
                bool leftPressed = IsTriggerPressed(state.Gamepad.bLeftTrigger);
                bool rightPressed = IsTriggerPressed(state.Gamepad.bRightTrigger);

                if (TranslateXtoMKB::lefthanded == 1) {
                    Xaxis = state.Gamepad.sThumbLX;
                    Yaxis = state.Gamepad.sThumbLY;
                    scrollXaxis = state.Gamepad.sThumbRX;
                    scrollYaxis = state.Gamepad.sThumbRY;
                }
                else
                {
                    Xaxis = state.Gamepad.sThumbRX;
                    Yaxis = state.Gamepad.sThumbRY;
                    scrollXaxis = state.Gamepad.sThumbLX;
                    scrollYaxis = state.Gamepad.sThumbLY;
                }

                delta = CalculateUltimateCursorMove(
                    Xaxis, Yaxis,
                    radial_deadzone, axial_deadzone, max_threshold,
                    curve_slope, curve_exponent,
                    sensitivity, accel_multiplier
                ); 
                if (delta.x != 0 || delta.y != 0) {
                    Xf += delta.x;
                    Yf += delta.y;
                    TranslateXtoMKB::RefreshWindow = 1;
                    TranslateXtoMKB::SendMouseClick(delta.x, delta.y, 8);
                }

                if (leftPressedold)
                {
                    if (!leftPressed)
                    {

                        TranslateXtoMKB::SendMouseClick(Xf, Yf, 6); //double click
                        leftPressedold = false;
                    }
                }
                else if (leftPressed)
                {
                    if (leftPressedold == false)
                    {
                        TranslateXtoMKB::SendMouseClick(Xf, Yf, 5); //4 skal vere 3
                        leftPressedold = true;
                    }
                }

                if (rightPressedold)
                {
                    if (!rightPressed)
                    {
                        TranslateXtoMKB::SendMouseClick(Xf, Yf, 4);
                        rightPressedold = false;
                    }
                } //if rightpress
                else if (rightPressed)
                {
                    if (rightPressedold == false)
                    {
                        TranslateXtoMKB::SendMouseClick(Xf, Yf, 3);
                        rightPressedold = true;

                    }
                }

                //buttons
                if (oldscrollleft)
                {
                    if (scrollXaxis < -deadzonelimit) //left
                    {
                    }
                    else {
                        oldscrollleft = false;
                        ButtonStateImpulse(TranslateXtoMKB::stickleftmapping, false, 99);//down
                    }
                }
                else if (scrollXaxis < -deadzonelimit) //left
                {
                    oldscrollleft = true;

                    ButtonStateImpulse(TranslateXtoMKB::stickleftmapping, true, 99);//down
                }

                if (oldscrollright)
                {
                    if (scrollXaxis > deadzonelimit) //left
                    {
                    }
                    else {
                        oldscrollright = false;
                        ButtonStateImpulse(TranslateXtoMKB::stickrightmapping, false, 99);//down
                    }
                }
                else if (scrollXaxis > deadzonelimit) //left
                {
                    oldscrollright = true;
                    ButtonStateImpulse(TranslateXtoMKB::stickrightmapping, true, 99);//down
                }

                if (oldscrollup)
                {
                    if (scrollYaxis > deadzonelimit) //left
                    {
                    }
                    else {
                        oldscrollup = false;
                        ButtonStateImpulse(TranslateXtoMKB::stickupmapping, false, 99);//down
                    }
                }
                else if (scrollYaxis > deadzonelimit) //left
                {
                    oldscrollup = true;
                    ButtonStateImpulse(TranslateXtoMKB::stickupmapping, true, 99);//down
                }

                if (oldscrolldown)
                {
                    if (scrollYaxis < -deadzonelimit) //left
                    {
                    }
                    else {
                        oldscrolldown = false;
                        ButtonStateImpulse(TranslateXtoMKB::stickdownmapping, false, 99);//down
                    }
                }
                else if (scrollYaxis < -deadzonelimit) //left
                {
                    oldscrolldown = true;
                    ButtonStateImpulse(TranslateXtoMKB::stickdownmapping, true, 99);//down
                }

                if (oldA)
                {
                    if (buttons & XINPUT_GAMEPAD_A)
                    {
                    }
                    else {
                        oldA = false;

                        ButtonStateImpulse(TranslateXtoMKB::Amapping, false, 0);//release
                    }

                }
                else if (buttons & XINPUT_GAMEPAD_A)
                {
                    oldA = true;
                    if (ScanThread::scanoption)
                    {
                        bool found = ScanThread::ButtonPressed(0);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::Amapping, true, 0);//down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::Amapping, true, 0);
                    TranslateXtoMKB::RefreshPoint = 2;
                }


                if (oldB)
                {
                    if (buttons & XINPUT_GAMEPAD_B)
                    {
                    }
                    else {
                        oldB = false;
                        ButtonStateImpulse(TranslateXtoMKB::Bmapping, false,1);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_B)
                {
                    oldB = true;
                    if (ScanThread::scanoption && modeMT != 2)
                    {
                        bool found = ScanThread::ButtonPressed(1);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::Bmapping, true, 1);//down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::Bmapping, true, 1);//down
                    TranslateXtoMKB::RefreshPoint = 2;
                }


                if (oldX)
                {
                    if (buttons & XINPUT_GAMEPAD_X)
                    {
                    }
                    else {
                        oldX = false;
                        ButtonStateImpulse(TranslateXtoMKB::Xmapping, false, 2);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_X)
                {
                    oldX = true;
                    if (ScanThread::scanoption && modeMT != 2)
                    {
                        bool found = ScanThread::ButtonPressed(2);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::Xmapping, true, 2);//down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::Xmapping, true, 2);//down
                    TranslateXtoMKB::RefreshPoint = 2;
                }


                if (oldY)
                {
                    if (buttons & XINPUT_GAMEPAD_Y)
                    {
                    }
                    else {
                        oldY = false;
                        ButtonStateImpulse(TranslateXtoMKB::Ymapping, false, 3);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_Y)
                {
                    oldY = true;
                    if (ScanThread::scanoption && modeMT != 2)
                    {
                        bool found = ScanThread::ButtonPressed(3);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::Ymapping, true, 3);//down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::Ymapping, true, 3);//down
                    TranslateXtoMKB::RefreshPoint = 2;
                }


                if (oldC)
                {
                    if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                    {
                    }
                    else {
                        oldC = false;
                        ButtonStateImpulse(TranslateXtoMKB::RSmapping, false, 99); //release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                {
                    oldC = true;
                    if (ScanThread::scanoption)
                    {
                        bool found = ScanThread::ButtonPressed(4);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::RSmapping, true, 99); //down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::RSmapping, true, 99); //down
                    TranslateXtoMKB::RefreshPoint = 10;
                }


                if (oldD)
                {
                    if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
                    {
                    }
                    else {
                        oldD = false;
                        ButtonStateImpulse(TranslateXtoMKB::LSmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
                {
                    oldD = true;
                    if (ScanThread::scanoption)
                    {
                        bool found = ScanThread::ButtonPressed(5);
                        if (!found)
                            ButtonStateImpulse(TranslateXtoMKB::LSmapping, true, 99);//down
                    }
                    else ButtonStateImpulse(TranslateXtoMKB::LSmapping, true, 99);//down
                    TranslateXtoMKB::RefreshPoint = 10;
                }


                if (oldleft)
                {
                    if (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
                    {
                    }
                    else {
                        oldleft = false;
                        ButtonStateImpulse(TranslateXtoMKB::leftmapping, false, 99); //release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
                {
                    oldleft = true;
                    ButtonStateImpulse(TranslateXtoMKB::leftmapping, true, 99);//down
                }


                if (oldright)
                {
                    if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
                    {
                    }
                    else {
                        oldright = false;
                        ButtonStateImpulse(TranslateXtoMKB::rightmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
                {
                    oldright = true;
                    ButtonStateImpulse(TranslateXtoMKB::rightmapping, true, 99);//down
                }


 

                if (oldstartoptions) //toggle fake cursor
                {
                    if (oldstart && oldoptions)
                    {
                    }
                    else
                    {
                        oldstartoptions = false;
                    }
                }
                else if (oldstart && oldoptions)//fake cursor toggle
                {
                    Proto::FakeCursor::SetCursorVisibility(!Proto::FakeCursor::GetCursorVisibility());
                    Proto::FakeCursor::Showmessage = 3;
                    TranslateXtoMKB::RefreshPoint = 1;
                    tick = 0;
                    oldstartoptions = true;
                }
                if (oldstart)
                {

                    if (buttons & XINPUT_GAMEPAD_START)
                    {
                        if (startbuttontimer < 1500) //delay setting menu. hold button to show
                            startbuttontimer++;
                        else 
                        { 
                            Proto::FakeCursor::Showmessage = 7; //adjust sensitivity
                            TranslateXtoMKB::RefreshPoint = 1;
                        }
                    }
                    else {
                        Proto::FakeCursor::Showmessage = 0; //adjust sensitivity
                        TranslateXtoMKB::RefreshPoint = 1;
                        oldstart = false;
                        ButtonStateImpulse(TranslateXtoMKB::startmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_START)
                {
                    startbuttontimer = 0;
                    oldstart = true;
                    ButtonStateImpulse(TranslateXtoMKB::startmapping, true, 99);//down

                }

                if (oldoptions)
                {
                    if (buttons & XINPUT_GAMEPAD_BACK)
                    {
                        if (backbuttontimer < 1500)
                            backbuttontimer ++;
                        else 
                        {
                            Proto::FakeCursor::Showmessage = 6; //adjust sensitivity
                            TranslateXtoMKB::RefreshPoint = 1;
                        }
                    }
                    else {
                        Proto::FakeCursor::Showmessage = 0; //adjust sensitivity
                        TranslateXtoMKB::RefreshPoint = 1;
                        oldoptions = false;
                        ButtonStateImpulse(TranslateXtoMKB::optionmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_BACK)
                {
                    backbuttontimer = 0;
                    oldoptions = true;
                    ButtonStateImpulse(TranslateXtoMKB::optionmapping, true, 99);//down
                }
                if (oldup)
                {
                    if (buttons & XINPUT_GAMEPAD_DPAD_UP)
                    {
                    }
                    else {
                        if (Proto::FakeCursor::Showmessage == 6)
                        {
                            if (TranslateXtoMKB::Sens < 50)
                            {
                                TranslateXtoMKB::Sens++;
                                Proto::FakeCursor::Showmessage = 0;
                                ScreenshotInput::TranslateXtoMKB::RefreshPoint = 1;
                            }
                        }
                        if (Proto::FakeCursor::Showmessage == 7)
                        {
                            if (TranslateXtoMKB::Sensmult < 50)
                            {
                                TranslateXtoMKB::Sensmult++;
                                Proto::FakeCursor::Showmessage = 0;
                                ScreenshotInput::TranslateXtoMKB::RefreshPoint = 1;
                            }
                        }
                        oldup = false;
                        ButtonStateImpulse(TranslateXtoMKB::upmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_DPAD_UP)
                {
                    oldup = true;
                    ButtonStateImpulse(TranslateXtoMKB::upmapping, true, 99);//down
                }


                if (olddown)
                {
                    if (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
                    {
                    }
                    else
                    {
                        if (Proto::FakeCursor::Showmessage == 6)
                        {
                            if (TranslateXtoMKB::Sens > 1)
                            {
                                TranslateXtoMKB::Sens--;
                                Proto::FakeCursor::Showmessage = 0;
                                ScreenshotInput::TranslateXtoMKB::RefreshPoint = 1;
                            }
                        }
                        if (Proto::FakeCursor::Showmessage == 7)
                        {
                            if (TranslateXtoMKB::Sensmult > 1)
                            {
                                TranslateXtoMKB::Sensmult--;
                                Proto::FakeCursor::Showmessage = 0;
                                ScreenshotInput::TranslateXtoMKB::RefreshPoint = 1;
                            }
                        }
                        olddown = false;
                        ButtonStateImpulse(TranslateXtoMKB::downmapping, false, 99);//release
                    }
                }
                else if (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
                {
                    olddown = true;
                    ButtonStateImpulse(TranslateXtoMKB::downmapping, true, 99);//down
                }
            } //if mode above 0
        } //if controller
        else { //no controller
            Proto::FakeCursor::Showmessage = 1;
            TranslateXtoMKB::RefreshPoint = 1;
            tick = 0;
        }
        if (tick < updatewindowtick)
            tick++;
        else { //need to update hwnd and bounds periodically
            // EnterCriticalSection(&ScanThread::critical);
            Proto::HwndSelector::UpdateMainHwnd(false);
            Proto::HwndSelector::UpdateWindowBounds();
            TranslateXtoMKB::RefreshPoint = 1;
            tick = 0;
        }

        if (Proto::FakeCursor::Showmessage != 0 && Proto::FakeCursor::Showmessage != 6 && Proto::FakeCursor::Showmessage != 7) { //drawing messages or something
            if (counter < 1000) {
                counter++;
            }
            else {
                Proto::FakeCursor::Showmessage = 0;
                TranslateXtoMKB::RefreshPoint = 1;
                counter = 0;
            }
        }
        if (!ScanThread::scanoption)
        {
           // Proto::FakeCursor::Showmessage = TranslateXtoMKB::showmessage;
        }
        if (modeMT > 0) {
            Sleep(1);
        }
        else Sleep(10);


        return;
    }

    void TranslateXtoMKB::Initialize(HMODULE hModule)
    {
        g_hModule = hModule;
        InstanceID = Proto::StateInfo::info.instanceIndex;
        Proto::AddThreadToACL(GetCurrentThreadId());
        //Sleep(50);
        return;
    }
}