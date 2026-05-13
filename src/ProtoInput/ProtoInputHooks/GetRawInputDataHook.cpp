#include <windows.h>
#include "GetRawInputDataHook.h"
#include "RegisterRawInputHook.h"
#include <cassert>
#include "RawInput.h"
#include "GtoMnK_RawInputHooks.h" //GtoMnK_RawInput.cpp
#include "GtoMnK_RawInput.h" //GtoMnK_RawInput.cpp//TranslateXtoMKB.h
//#include "TranslateXtoMKB.h" //GtoMnK_RawInput.cpp//TranslateXtoMKB.h

namespace Proto
{
	

UINT WINAPI Hook_GetRawInputData(
	HRAWINPUT hRawInput,
	UINT      uiCommand,
	LPVOID    pData,
	PUINT     pcbSize,
	UINT      cbSizeHeader
)
{

	unsigned int h = (unsigned int)hRawInput; // Only care about first 4 bytes.
	bool hasSignature = (h & 0xFF000000) == 0xAB000000;
	//MessageBoxA(NULL, "WARNING: GetRawInputData doesn't have the signature. This means Proto Input won't work properly. Please report this to the developer.", "Proto Input", MB_OK | MB_ICONWARNING);
	if (!hasSignature)
	{
		
		// printf("GetRawInputData doesn't have signature: h = 0x%X\n", h);
		return GetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
	}
	else
	{

		if (RawInput::TranslateXinputtoMKB2) // TranslateXinputtoMKB2 is just a copy of TranslateXinputtoMKB
		{
			UINT handleValue = (UINT)(UINT_PTR)hRawInput;
			UINT bufferIndex = handleValue & 0x00FFFFFF;
			if (bufferIndex >= 20) {
				return GetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
			}

			if (pData == NULL) {
				*pcbSize = sizeof(RAWINPUT);
				return 0;
			}

			RAWINPUT* storedData = &RawInput::inputBuffer[RawInput::bufferCounter];
			memcpy(pData, storedData, sizeof(RAWINPUT));
			return sizeof(RAWINPUT);
		}
		// printf("$");

		auto index = h & 0x00FFFFFF;


		if (pData == NULL)
		{
			*pcbSize = uiCommand == RID_INPUT ? sizeof(RAWINPUT) : sizeof(RAWINPUTHEADER);
			return 0;
		}
		else
		{

			if (uiCommand == RID_INPUT)
			{
				RAWINPUT* ptr = (RAWINPUT*)pData;
				*ptr = RawInput::inputBuffer[index];
				return *pcbSize;
			}
			else
			{
				RAWINPUTHEADER* ptr = (RAWINPUTHEADER*)pData;
				*ptr = RawInput::inputBuffer[index].header;
				return *pcbSize;
			}
		}
	}
}

void GetRawInputDataHook::InstallImpl()
{
		hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetRawInputData", Hook_GetRawInputData));
		
}

void GetRawInputDataHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}