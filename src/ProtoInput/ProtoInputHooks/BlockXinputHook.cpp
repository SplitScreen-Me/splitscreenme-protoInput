#include "BlockXinputHook.h"
#include <imgui.h>
#include <Xinput.h>

namespace Proto
{

	DWORD WINAPI Hook_XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		return ERROR_SUCCESS;
	}
	DWORD WINAPI Hook_XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		return ERROR_SUCCESS;
	}

	DWORD WINAPI Hook_XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
	{
		return ERROR_SUCCESS;
	}
	

	void BlockXinputHook::ShowGuiStatus()
	{

		ImGui::Separator();
		ImGui::Text("This hook blocks Xinput Controllers from being discovered by game.");
	}

	void BlockXinputHook::InstallImpl()
	{
		// Some games (e.g. Terraria) haven't loaded the dlls when we inject hooks. So load all XInput dlls.
		const wchar_t* xinputNames[] = {
					L"xinput1_3.dll", L"xinput1_4.dll", L"xinput1_2.dll", L"xinput1_1.dll", L"xinput9_1_0.dll"
		};
		for (const auto xinputName : xinputNames)
		{
			if (LoadLibraryW(xinputName) == nullptr)
			{
				fprintf(stderr, "Not hooking %ws as failed to load dll\n", xinputName);
				continue;
			}

			if (GetModuleHandleW(xinputName) == nullptr)
			{
				fprintf(stderr, "Not hooking %ws as failed get module\n", xinputName);
				continue;
			}

			hookInfos.push_back(std::get<1>(InstallNamedHook(xinputName, "XInputGetState", Hook_XInputGetState)));
			hookInfos.push_back(std::get<1>(InstallNamedHook(xinputName, "XInputSetState", Hook_XInputSetState)));
		}
		//XinputGetStateEx (hidden call, ordinal 100). Only present in xinput1_4.dll and xinput1_3.dll. Used by EtG and DoS2
		//DWORD as 1st param and similar structure pointer for 2nd param (with an extra DWORD at the end). Can be treated as a normal XINPUT_STATE.
		if (nullptr != LoadLibraryW(L"xinput1_4.dll"))
		{
			hookInfos.push_back(std::get<1>(InstallNamedHook(L"xinput1_4.dll", (LPCSTR)(100), Hook_XInputGetStateEx, true)));
		}
		if (nullptr != LoadLibraryW(L"xinput1_3.dll"))
		{
			hookInfos.push_back(std::get<1>(InstallNamedHook(L"xinput1_3.dll", (LPCSTR)(100), Hook_XInputGetStateEx, true)));

			XInputGetStateExPtr = t_XInputGetStateEx(GetProcAddress(GetModuleHandleW(L"xinput1_3.dll"), (LPCSTR)(100)));
			XInputGetStatePtr = t_XInputGetState(GetProcAddress(GetModuleHandleW(L"xinput1_3.dll"), "XInputGetState"));
			XInputSetStatePtr = t_XInputSetState(GetProcAddress(GetModuleHandleW(L"xinput1_3.dll"), "XInputSetState"));

			if (XInputGetStateExPtr == nullptr)
				MessageBoxA(NULL, "XInputGetStateExPtr is null", "Error", MB_OK);

			if (XInputGetStatePtr == nullptr)
				MessageBoxA(NULL, "XInputGetStatePtr is null", "Error", MB_OK);

			if (XInputSetStatePtr == nullptr)
				MessageBoxA(NULL, "XInputSetStatePtr is null", "Error", MB_OK);

			if (XInputGetCapabilitiesPtr == nullptr)
				MessageBoxA(NULL, "XInputGetCapabilitiesPtr is null", "Error", MB_OK);
	}

	void BlockXinputHook::UninstallImpl()
	{
		UninstallHook(&hookInfoBlockXinput);
	}

}
