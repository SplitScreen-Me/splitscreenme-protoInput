#include "FakeCursor.h"
#include <windows.h>
#include "InstallHooks.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "ScanThread.h"
#include "TranslateXtoMKB.h"
#include "RawInput.h"
#include "SetCursorPosHook.h"
#include "XinputHook.h"
#include <string>

namespace Proto
{

FakeCursor FakeCursor::state{};
int FakeCursor::Showmessage = 0;
bool FakeCursor::DrawFakeCursorFix;


LRESULT WINAPI FakeCursorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
    	break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LONG fakeCursorMinX = 0, fakeCursorMaxX = 0, fakeCursorMinY = 0, fakeCursorMaxY = 0;

BOOL CALLBACK EnumWindowsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFO info = { sizeof(info) };
    if (GetMonitorInfo(hMonitor, &info))
    {
#undef min // Thanks Windows.h
#undef max
        fakeCursorMinX = std::min(fakeCursorMinX, info.rcMonitor.left);
        fakeCursorMinY = std::min(fakeCursorMinY, info.rcMonitor.top);
        fakeCursorMaxX = std::max(fakeCursorMaxX, info.rcMonitor.right);
        fakeCursorMaxY = std::max(fakeCursorMaxY, info.rcMonitor.bottom);
    }
    return true;
}
void DrawRedX(HDC hdc, int x, int y) //blue
{
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, x - 15, y - 15, NULL);
    LineTo(hdc, x + 15, y + 15);

    MoveToEx(hdc, x + 15, y - 15, NULL);
    LineTo(hdc, x - 15, y + 15);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    return;
}
void DrawBlueCircle(HDC hdc, int x, int y) //red
{
    // Create a NULL brush (hollow fill)
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, x - 15, y - 15, x + 15, y + 15);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}
void DrawGreenTriangle(HDC hdc, int x, int y)
{
    // Use a NULL brush for hollow
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    POINT pts[3];
    pts[0].x = x; pts[0].y = y - 10;        // top center
    pts[1].x = x - 10; pts[1].y = y + 10;   // bottom left
    pts[2].x = x + 10; pts[2].y = y + 10;   // bottom right

    Polygon(hdc, pts, 3);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void DrawPinkSquare(HDC hdc, int x, int y)
{
    // Create a NULL brush (hollow fill)
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 192, 203));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw hollow rectangle (square) 20x20
    Rectangle(hdc, x - 15, y - 15, x + 15, y + 15);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}


void FakeCursor::DrawMessage(HDC hdc, HWND window, HBRUSH Brush, int message) 
{
    POINT here = { 0,0 };
    ClientToScreen(window, &here);

    if (oldmessage != message)
    {
       // RECT fill{ here.x + 20, here.y + 20, 500, 500 };
       // FillRect(hdc, &fill, Brush); // Note: window, not screen coordinates!
       RECT wholewindow;
       GetClientRect(pointerWindow, &wholewindow);
       wholewindow.bottom = wholewindow.bottom / 2;
       FillRect(hdc, &wholewindow, Brush);
      // MessageBoxA(NULL, "Message Erased!", "Debug", MB_OK);
       messageshown = false;
       ScreenshotInput::TranslateXtoMKB::RefreshWindow = 1; //redraw cursor
    }
    if (!messageshown)
    { 
        if (message == 1){
            TextOutW(hdc, here.x + 20, here.y + 20, TEXT("DISCONNECTED!"), 14); //14
		    messageshown = true;
	    }
        if (message == 2) {
            TextOutW(hdc, here.x + 20, here.y + 20, TEXT("GUI TOGGLE!"), 11); //14
            messageshown = true;
        }
        if (message == 3) {
            TextOutW(hdc, here.x + 20, here.y + 20, TEXT("SHOWCURSOR TOGGLE!"), 18); //14
            messageshown = true;
        }
        if (message == 4) {
            TextOutW(hdc, here.x + 20, here.y + 20, TEXT("LOCK TOGGLED!"), 13); //14
            messageshown = true;
        }
        if (message == 5) {
            TextOutW(hdc, here.x + 20, here.y + 20, TEXT("Window top!"), 11); //14
            messageshown = true;
        }
        if (message == 6) {
            wchar_t buffer[25];
            swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"Sens Flat Adjust! (%d)", ScreenshotInput::TranslateXtoMKB::Sens);
            TextOutW(hdc, here.x + 20, here.y + 20, buffer, (int)wcslen(buffer));
            TextOutW(hdc, here.x + 20, here.y + 40, TEXT("Press Up or Down!"), 17); //14
            messageshown = true;
        }
        if (message == 7) {
            wchar_t buffer[25];
            swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"Sens Curve Adjust! (%d)", ScreenshotInput::TranslateXtoMKB::Sensmult);
            TextOutW(hdc, here.x + 20, here.y + 20, buffer, (int)wcslen(buffer));
            TextOutW(hdc, here.x + 20, here.y + 40, TEXT("Press Up or Down!"), 17);
            messageshown = true;
        }
        if (ScreenshotInput::TranslateXtoMKB::SaveBmps) {
            TextOutW(hdc, here.x + 20, here.y + 0, TEXT("BMP SAVE MODE!"), 14); //14
            messageshown = true;
        }
        if (message == 10) {
            TextOutW(hdc, here.x + 20, here.y + 25, TEXT("A MAPPED TO SPOT!"), 17); //14
            messageshown = true;
        }
        if (message == 11) {
            TextOutW(hdc, here.x + 20, here.y + 25, TEXT("B MAPPED TO SPOT!"), 17); //14
            messageshown = true;
        }
        if (message == 12) {
            TextOutW(hdc, here.x + 20, here.y + 25, TEXT("X MAPPED TO SPOT!"), 17); //14
            messageshown = true;
        }
        if (message == 13) {
            TextOutW(hdc, here.x + 20, here.y + 25, TEXT("Y MAPPED TO SPOT!"), 17); //14
            messageshown = true;
        }
    }
    oldmessage = message;

}
void FakeCursor::DrawFoundSpots(HDC hdc, POINT spotA, POINT spotB, POINT spotX, POINT spotY,HWND window, HBRUSH Brush)
{
	bool windowmoved = false;
    bool erasedA = false;
    bool erased = false;
    bool erasedB = false;
    bool erasedX = false;
    bool erasedY = false;
    //detect window change pos
    POINT testpos;
	ClientToScreen(window, &testpos);
    if (testpos.x < OldTestpos.x || testpos.y < OldTestpos.y || testpos.x > OldTestpos.x || testpos.y > OldTestpos.y)
    {
        erased = true;
	}

    //point coordinate changes
    if (OldspotA.x != spotA.x || OldspotA.y != spotA.y)
    {
        RECT fill{ OldspotA.x - 20, OldspotA.y - 20, OldspotA.x + 20, OldspotA.y + 20 };
        FillRect(hdc, &fill, transparencyBrush); // Note: window, not screen coordinates!
        erasedA = true;
    }

    if (OldspotB.x != spotB.x || OldspotB.y != spotB.y) //|| windowmoved)
    {
        RECT fill{ OldspotB.x - 20, OldspotB.y - 20, OldspotB.x + 20, OldspotB.y + 20 };
        FillRect(hdc, &fill, transparencyBrush); // Note: window, not screen coordinates!
        erasedB = true;
    }
    if (OldspotX.x != spotX.x || OldspotX.y != spotX.y) //|| windowmoved)
    {
        RECT fill{ OldspotX.x - 20, OldspotX.y - 20, OldspotX.x + 20, OldspotX.y + 20 };
        FillRect(hdc, &fill, transparencyBrush); // Note: window, not screen coordinates!
        erasedX = true;
    }
    if (OldspotY.x != spotY.x || OldspotY.y != spotY.y) //|| windowmoved)
    {
        RECT fill{ OldspotY.x - 20, OldspotY.y - 20, OldspotY.x + 20, OldspotY.y + 20 };
        FillRect(hdc, &fill, transparencyBrush); // Note: window, not screen coordinates!
        erasedY = true;
        
    }


    if (erased == true)
    {
        RECT wholewindow;
        GetClientRect(pointerWindow, &wholewindow);

        FillRect(hdc, &wholewindow, Brush);

        erasedA = true;
        erasedB = true;
        erasedX = true;
        erasedY = true;

        ScreenshotInput::TranslateXtoMKB::RefreshWindow = 1; //redraw cursor
    }


    OldspotA.x = spotA.x;
    OldspotA.y = spotA.y;

    OldspotB.x = spotB.x;
    OldspotB.y = spotB.y;

    OldspotX.x = spotX.x;
    OldspotX.y = spotX.y;

    OldspotY.x = spotY.x;
    OldspotY.y = spotY.y;


    if (spotA.x != 0 && spotA.y != 0 && erasedA == true)
    { 
        POINT drawthere = spotA;
        ClientToScreen(window, &drawthere); //A
        DrawRedX(hdc, drawthere.x, drawthere.y);
    }


    if (spotB.x != 0 && spotB.y != 0 && erasedB == true)
    {
        POINT drawthere = spotB;
        ClientToScreen(window, &drawthere); //A
        DrawBlueCircle(hdc, drawthere.x, drawthere.y);
    }


    if (spotX.x != 0 && spotX.y != 0 && erasedX == true)
    {
        POINT drawthere = spotX;
        ClientToScreen(window, &drawthere); //A
        DrawPinkSquare(hdc, drawthere.x, drawthere.y);
        
    }


    if (spotY.x != 0 && spotY.y != 0 && erasedY == true)
    {
        POINT drawthere = spotY;
        ClientToScreen(window, &drawthere); //A
        DrawGreenTriangle(hdc, drawthere.x, drawthere.y);
    }
	OldTestpos = testpos;
}

void FakeCursor::DrawPointsandMessages() //only on Xtranslate
{
    if (ScreenshotInput::ScanThread::scanoption)
    {
        EnterCriticalSection(&ScreenshotInput::ScanThread::critical);
        selectorhwnd = (HWND)HwndSelector::GetSelectedHwnd();
        // FakeCursor::Showmessage = ScreenshotInput::TranslateXtoMKB::showmessage;
        POINT Apos = { ScreenshotInput::ScanThread::PointA.x, ScreenshotInput::ScanThread::PointA.y };
        POINT Bpos = { ScreenshotInput::ScanThread::PointB.x, ScreenshotInput::ScanThread::PointB.y };
        POINT Xpos = { ScreenshotInput::ScanThread::PointX.x, ScreenshotInput::ScanThread::PointX.y };
        POINT Ypos = { ScreenshotInput::ScanThread::PointY.x, ScreenshotInput::ScanThread::PointY.y };
        FakeCursor::DrawFoundSpots(hdc, Apos, Bpos, Xpos, Ypos, selectorhwnd, transparencyBrush);
        FakeCursor::DrawMessage(hdc, selectorhwnd, transparencyBrush, FakeCursor::Showmessage);
        LeaveCriticalSection(&ScreenshotInput::ScanThread::critical);
    }
    else if (RawInput::TranslateXinputtoMKB)
        DrawMessage(hdc, (HWND)HwndSelector::GetSelectedHwnd(), transparencyBrush, FakeCursor::Showmessage);

}
void FakeCursor::DrawCursor()
{

    POINT pos = { FakeMouseKeyboard::GetMouseState().x,FakeMouseKeyboard::GetMouseState().y };

    if (XinputHook::TranslateMKBtoXinput)
    {
        pos.x = SetCursorPosHook::mousesethere.x;
        pos.y = SetCursorPosHook::mousesethere.y;
    }
    ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), &pos);
    ScreenToClient(pointerWindow, &pos);

    if (oldHadShowCursor) //erase cursor
    {
        RECT fill{ oldX, oldY, oldX + cursorWidth, oldY + cursorHeight };
        FillRect(hdc, &fill, transparencyBrush); // Note: window, not screen coordinates!
    }

    oldHadShowCursor = showCursor;


    if (DrawFakeCursorFix)
    {
        pos.x -= cursoroffsetx;
        pos.y -= cursoroffsety;

        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;

        //offsetSET is stepping each cursor update: step 0 check size //step 1 check offset //step 2 only draw
        if (showCursor)// && hdc && hCursor
        {
            if (DrawIconEx(hdc, pos.x, pos.y, hCursor, cursorWidth, cursorHeight, 0, transparencyBrush, DI_NORMAL))
            {
                if (hCursor != oldhCursor && offsetSET > 1 && nochange == false)
                {
                    offsetSET = 0;
                }
                if (offsetSET == 1 && hCursor != LoadCursorW(NULL, IDC_ARROW) && IsWindowVisible(pointerWindow)) //offset setting
                {
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    HBITMAP hbmScreen = CreateCompatibleBitmap(hdc, cursorWidth, cursorHeight);
                    SelectObject(hdcMem, hbmScreen);
                    BitBlt(hdcMem, 0, 0, cursorWidth, cursorHeight, hdc, pos.x, pos.y, SRCCOPY);
                    cursoroffsetx = -1;
                    cursoroffsety = -1;
                    int leftcursoroffsetx = 0;
                    int leftcursoroffsety = -1;
                    int rightcursoroffsetx = 0;
                    // Scanning
                    for (int y = 0; y < cursorHeight; y++)
                    {
                        for (int x = 0; x < cursorWidth; x++)
                        {
                            COLORREF pixelColor = GetPixel(hdcMem, x, y); // scan from left and find common y to use in next scan also
                            if (pixelColor != transparencyKey)
                            {
                                leftcursoroffsetx = x;
                                leftcursoroffsety = y;
                                break;
                            }
                        }
                        if (leftcursoroffsetx != -1) break;
                    }


                    for (int x = cursorWidth - 1; x >= 0; x--)
                    {
                        COLORREF pixelColor = GetPixel(hdcMem, x, leftcursoroffsety); // scan from right
                        if (pixelColor != transparencyKey)
                        {
                            rightcursoroffsetx = cursorWidth - x;
                            break;
                        }
                    }
                    //Adjusting possible here if symmetric cursor is not found
                    if (leftcursoroffsetx == rightcursoroffsetx - 1 || leftcursoroffsetx == rightcursoroffsetx + 1 || leftcursoroffsetx == rightcursoroffsetx) //check for symmetric first only if Y offset is small
                    {
                        cursoroffsety = cursorHeight / 2;
                        cursoroffsetx = cursorWidth / 2;
                    }
                    else if (leftcursoroffsety > 2 || leftcursoroffsetx > 2) //is there any other offsets?
                    {
                        cursoroffsetx = leftcursoroffsetx;
                        cursoroffsety = leftcursoroffsety;
                        nochange = true;
                    }

                    else { //no offsets
                        cursoroffsetx = 0;
                        cursoroffsety = 0;
                    }
                    offsetSET++; //offset set to 2 should do drawing only now
                    DeleteDC(hdcMem);
                    DeleteObject(hbmScreen);



                }
                if (offsetSET == 0 && hCursor != LoadCursorW(NULL, IDC_ARROW) && IsWindowVisible(pointerWindow)) //size setting
                {
                    ICONINFO ii;
                    BITMAP bitmap;
                    cursoroffsetx = 0;
                    cursoroffsety = 0;
                    if (GetIconInfo(hCursor, &ii))
                    {
                        if (GetObject(ii.hbmMask, sizeof(BITMAP), &bitmap))
                        {
                            cursorWidth = bitmap.bmWidth;
                            if (ii.hbmColor == NULL)
                            {//For monochrome icons, the hbmMask is twice the height of the icon and hbmColor is NULL
                                cursorHeight = bitmap.bmHeight / 2;
                            }
                            else
                            {
                                cursorHeight = bitmap.bmHeight;
                            }
                            DeleteObject(ii.hbmColor);
                            DeleteObject(ii.hbmMask);
                        }

                    }
                    offsetSET++; //size set, doing offset next run
                }
                oldhCursor = hCursor;
            }
        }
    }
    else if (showCursor)
    {
        DrawIcon(hdc, pos.x, pos.y, hCursor);
    }
    oldX = pos.x;
    oldY = pos.y;
}

DWORD WINAPI FakeCursorDrawLoopThread(LPVOID lpParameter)
{
    printf("Fake cursor draw loop thread start\n");
    Proto::AddThreadToACL(GetCurrentThreadId());
    FakeCursor::state.StartDrawLoopInternal();

    return 0;
}

void FakeCursor::StartDrawLoopInternal()
{
    int tick = 1;

    if (Proto::RawInput::TranslateXinputtoMKB)
        ScreenshotInput::TranslateXtoMKB::Initialize(GetModuleHandle(NULL));

	while (true)
	{
        if (!Proto::RawInput::TranslateXinputtoMKB)
        { 
            std::unique_lock<std::mutex> lock(mutex);
		    conditionvar.wait(lock);
        
            DrawCursor();

            //TODO: is this ok? (might eat cpu)
            Sleep(drawingEnabled ? 12 : 500);
            tick = (tick + 1) % 200;
        }
        else 
        {
            ScreenshotInput::TranslateXtoMKB::ThreadFunction();
            if (ScreenshotInput::TranslateXtoMKB::RefreshPoint > 0)
            {
                DrawPointsandMessages();
                ScreenshotInput::TranslateXtoMKB::RefreshPoint--;
            }
            if (ScreenshotInput::TranslateXtoMKB::RefreshWindow > 0)
            {
                DrawCursor();
                ScreenshotInput::TranslateXtoMKB::RefreshWindow--;
            }
            tick = (tick + 1) % 2000; //guess this run about 10-12 times faster
        }

        if (tick == 0)
        { 
            // Nucleus can put the game window above the pointer without this
            SetWindowPos(pointerWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
        }
	}
}

void FakeCursor::StartInternal()
{
    Proto::AddThreadToACL(GetCurrentThreadId());

    const auto hInstance = GetModuleHandle(NULL);

    // Like a green screen
    transparencyBrush = (HBRUSH)CreateSolidBrush(transparencyKey);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = FakeCursorWndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = transparencyBrush;

    std::srand(std::time(nullptr));
    const std::wstring classNameStr = std::wstring{ L"ProtoInputPointer" } + std::to_wstring(std::rand());
    const wchar_t* className = classNameStr.c_str();

    wc.lpszClassName = className;
    wc.style = CS_OWNDC | CS_NOCLOSE;

    if (!RegisterClass(&wc))
    {
        fprintf(stderr, "Failed to open fake cursor window\n");
    }
    else
    {
        pointerWindow = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_NOINHERITLAYOUT | WS_EX_NOPARENTNOTIFY |
                                                          WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                                                          wc.lpszClassName, classNameStr.c_str(), 0,
                                                          0, 0, 200, 200,
                                                          nullptr, nullptr, hInstance, nullptr);

        SetWindowLongW(pointerWindow, GWL_STYLE, WS_VISIBLE | WS_DISABLED);
        SetLayeredWindowAttributes(pointerWindow, transparencyKey, 0, LWA_COLORKEY);

        // Nucleus can put the game window above the pointer without this
        SetWindowPos(pointerWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);

        // ShowWindow(pointerWindow, SW_SHOWDEFAULT);
        // UpdateWindow(pointerWindow);
        EnableDisableFakeCursor(drawingEnabled);

    	// Over every screen
        EnumDisplayMonitors(nullptr, nullptr, &EnumWindowsProc, 0);
        MoveWindow(pointerWindow, fakeCursorMinX, fakeCursorMinY, fakeCursorMaxX - fakeCursorMinX, fakeCursorMaxY - fakeCursorMinY, TRUE);

        hdc = GetDC(pointerWindow);

        //TODO: configurable cursor
        hCursor = LoadCursorW(NULL, IDC_ARROW);

        const auto threadHandle = CreateThread(nullptr, 0,
                              (LPTHREAD_START_ROUTINE)FakeCursorDrawLoopThread, GetModuleHandle(0), 0, 0);

        if (threadHandle != nullptr)
            CloseHandle(threadHandle);

    	// Want to avoid doing anything in the message loop that might cause it to not respond, as the entire screen will say not responding...
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
        	if (GetMessageW(&msg, pointerWindow, 0U, 0U))// Blocks
        	{
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }
        }
    }
}

DWORD WINAPI FakeCursorThreadStart(LPVOID lpParameter)
{
    printf("Fake Cursor thread start\n");
    FakeCursor::state.StartInternal();
    return 0;
}


void FakeCursor::EnableDisableFakeCursor(bool enable)
{
    state.drawingEnabled = enable;
	
    ShowWindow(state.pointerWindow, enable ? SW_SHOWDEFAULT : SW_HIDE);
    UpdateWindow(state.pointerWindow);
}

void FakeCursor::Initialise(HMODULE module)
{
	const auto threadHandle = CreateThread(nullptr, 0,
                 (LPTHREAD_START_ROUTINE)FakeCursorThreadStart, module, 0, 0);

    if (threadHandle != nullptr)
        CloseHandle(threadHandle);
}

}
