#pragma once
#include <Windows.h>

namespace ScreenshotInput {
    class RawInput {
	    public:
        static void TranslateKeyboardAction(int actionCode, int& outVkCode, bool& outIsExtended);
        static void GenerateRawMouseButton(int actionCode, bool press);
        static void GenerateRawKey(int vkCode, bool press, bool isExtended);
        static void SendActionDelta(int deltaX, int deltaY);
    };
}