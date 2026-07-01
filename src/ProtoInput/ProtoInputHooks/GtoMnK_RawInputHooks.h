#pragma once
#include <windows.h>

namespace ScreenshotInput {
	class RawInputHooks {
	public:
		static UINT WINAPI GetRawInputDataHookX(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);
		static BOOL WINAPI RegisterRawInputDevicesHookX(PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize);
		//static void InstallHooks();
	};
}
