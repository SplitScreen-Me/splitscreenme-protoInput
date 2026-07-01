#pragma once
#include <cstdint>
#include <windows.h>

namespace ProtoHost
{

extern HWND rawInputHwnd;

extern intptr_t lastKeypressKeyboardHandle;
extern intptr_t lastMouseClicked;

extern int lastVKcode;
//extern int Akey;
//extern int Bkey;
//extern int Xkey;
//extern int Ykey;
//extern int RSkey;
//extern int LSkey;

extern bool lockInputWithTheEndKey;
extern bool lockInputSuspendsExplorer;
extern bool freezeGameInputWhileInputNotLocked;

void InitialiseRawInput();

}
