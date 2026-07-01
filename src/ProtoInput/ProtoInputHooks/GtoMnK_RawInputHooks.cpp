//#include "pch.h"
#include "GtoMnK_RawInputHooks.h"
#include "RawInput.h"
#include "KeyboardButtonFilter.h"
//#include "Mouse.h"
//#include "Keyboard.h"
#include "EasyHook.h"
#include "gui.h"
// Thanks to ProtoInput.

namespace ScreenshotInput
{
   // HOOK_TRACE_INFO g_getRawInputDataHook = { NULL };
   // HOOK_TRACE_INFO g_registerRawInputDevicesHook = { NULL };

  //  void RawInputHooks::InstallHooks() {
   //     // Install GetRawInputData hook
  //      HMODULE hUser32 = GetModuleHandleA("user32");
   //     LhInstallHook(GetProcAddress(hUser32, "GetRawInputData"), GetRawInputDataHook, NULL, &g_getRawInputDataHook);
   //     LhInstallHook(GetProcAddress(hUser32, "RegisterRawInputDevices"), RegisterRawInputDevicesHook, NULL, &g_registerRawInputDevicesHook);
   //     ULONG ACLEntries[1] = { 0 };
   //     LhSetExclusiveACL(ACLEntries, 1, &g_getRawInputDataHook);
  //      LhSetExclusiveACL(ACLEntries, 1, &g_registerRawInputDevicesHook);
  //  }
}