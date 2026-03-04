// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include <iostream>
#include <BlackBone/Process/Process.h>
#include <BlackBone/Patterns/PatternSearch.h>
#include <BlackBone/Process/RPC/RemoteFunction.hpp>
#include <BlackBone/Syscalls/Syscall.h>
#include <imgui.h>
#include <easyhook.h>
#include "Gui.h"
#include "RawInput.h"
#include "HookManager.h"
#include "protoloader.h"
#include "Scaler.h"
#include "PipeCommunication.h"
#include "HwndSelector.h"
#include "FocusMessageLoop.h"
#include "Gui.h"
#include "FakeCursor.h"
#include "TranslateXtoMKB.h"
#include "ScanThread.h"

HMODULE Proto::hmodule;

DWORD WINAPI StartThread(LPVOID lpParameter)
{
    
    AllocConsole();
    FILE* f = new FILE();
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    // Proto::ConsoleHwnd = (intptr_t)FindWindowW(L"ConsoleWindowClass", NULL);
    Proto::ConsoleHwnd = (intptr_t)GetConsoleWindow();
    Proto::SetConsoleVisible(false);

    std::cout << "Hooks DLL loaded\n";

    Proto::HwndSelector::UpdateMainHwnd();

    Proto::FocusMessageLoop::SetupThread();

    Proto::FakeCursor::Initialise(Proto::hmodule);

    Proto::AddThreadToACL(GetCurrentThreadId());

    Proto::StartPipeCommunication(); 
    InitializeCriticalSection(&ScreenshotInput::ScanThread::critical);//must be placed before InitialiseRawInput
    Proto::RawInput::InitialiseRawInput();
	// Useful to add a pause if we need to attach a debugger
    
    InitializeCriticalSection(&ScreenshotInput::ScanThread::critical);

    std::cout << "Reached end of startup thread\n";
    return 0;
}
 
// EasyHook entry point
extern "C" __declspec(dllexport) void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * inRemoteInfo)
{
    printf("EasyHook NativeInjectionEntryPoint called\n");
}

EASYHOOK_BOOL_EXPORT EasyHookDllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

extern "C" BOOL WINAPI OpenXinputDllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	// Needed for EasyHook to initialise some stuff
    EasyHookDllMain(hModule, ul_reason_for_call, lpReserved);

    OpenXinputDllMain(hModule, ul_reason_for_call, lpReserved);
	
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        Proto::hmodule = hModule;
    		
         HANDLE hThread = CreateThread(nullptr, 0,
                                      (LPTHREAD_START_ROUTINE)StartThread, hModule, 0, 0);
         if (hThread != nullptr)
             CloseHandle(hThread);
             		
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
       
	
    return TRUE;
}
