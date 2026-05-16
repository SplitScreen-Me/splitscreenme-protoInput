#pragma once
#include <windows.h>
namespace Proto
{
#define MAX_GAMEPADS 32

	class HidRawDevice
	{
	private:
	public:
		static BOOL ReadDeviceName(int index, HANDLE handle);
		static BOOL ReadDeviceInfo(int index, HANDLE handle);
		static int FindIndex(HANDLE handle);
		static void searchgamepads();
		static TCHAR* deviceNames[MAX_GAMEPADS];
		static HANDLE deviceHandles[MAX_GAMEPADS];
		static UINT devicesActive;
	};
}