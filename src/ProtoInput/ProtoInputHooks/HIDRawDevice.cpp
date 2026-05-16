#include <windows.h>
#include "HIDRawDevice.h"
//stolen from https://github.com/aholmes/XboxController/blob/master/RawInput/RawInput.cpp

namespace Proto
{
	TCHAR* HidRawDevice::deviceNames[MAX_GAMEPADS];
	HANDLE HidRawDevice::deviceHandles[MAX_GAMEPADS];
	UINT HidRawDevice::devicesActive;

	BOOL HidRawDevice::ReadDeviceName(int index, HANDLE handle)
	{
		UINT size = 0;
		UINT result = GetRawInputDeviceInfo(handle,
			RIDI_DEVICENAME, NULL, &size);
		if (result != 0) {
			result = GetLastError();
			return false;
		}

		deviceNames[index] = (TCHAR*)HeapAlloc(GetProcessHeap(), 0, size * sizeof(TCHAR));
		result = GetRawInputDeviceInfo(handle,
			RIDI_DEVICENAME, deviceNames[index], &size);
		if (result < 0) {
			result = GetLastError();
			HeapFree(GetProcessHeap(), 0, deviceNames[index]);
			deviceHandles[index] = NULL;
			deviceNames[index] = NULL;
			return false;
		}

		deviceHandles[index] = handle;
		return TRUE;
	}


	BOOL HidRawDevice::ReadDeviceInfo(int index, HANDLE handle)
	{
		// May be a game pad
		RID_DEVICE_INFO* info;

		UINT size = 0; // sizeof(RID_DEVICE_INFO);
		UINT result = GetRawInputDeviceInfo(handle, RIDI_DEVICEINFO,
			NULL, &size);

		info = (RID_DEVICE_INFO*)HeapAlloc(GetProcessHeap(), 0, size);

		result = GetRawInputDeviceInfo(handle, RIDI_DEVICEINFO,
			info, &size);

		if (result != (UINT)-1) {
			if (info->hid.usUsagePage == 1 &&
				(info->hid.usUsage == 4 || info->hid.usUsage == 5)) {
				// It is a game pad!!!

				HeapFree(GetProcessHeap(), 0, info);
				return ReadDeviceName(index, handle);;
			}
		}
		else {
			result = (UINT)GetLastError();
		}
		HeapFree(GetProcessHeap(), 0, info);
		return FALSE;
	}

	int HidRawDevice::FindIndex(HANDLE handle)
	{
		for (int i = 0; i < MAX_GAMEPADS; i++)
			if (deviceHandles[i] == handle)
				return i;

		return -1;
	}
	void HidRawDevice::searchgamepads()
	{
		UINT numdevices;
		GetRawInputDeviceList(NULL, &numdevices, sizeof(RAWINPUTDEVICELIST));
		RAWINPUTDEVICELIST* list = (RAWINPUTDEVICELIST*)HeapAlloc(GetProcessHeap(), 0,
			numdevices * sizeof(RAWINPUTDEVICELIST));
		GetRawInputDeviceList(list, &numdevices, sizeof(RAWINPUTDEVICELIST));

		for (UINT i = 0; i < numdevices; i++) {
			if (list[i].dwType == RIM_TYPEHID) {
				if (HidRawDevice::ReadDeviceInfo(HidRawDevice::devicesActive, list[i].hDevice))
					HidRawDevice::devicesActive++;
			}

		}
	}
}