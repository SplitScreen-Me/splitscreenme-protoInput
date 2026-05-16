#pragma once
#include <vector>
#include <Windows.h>

namespace ScreenshotInput {
    class RawInput {
    private:
        static void TranslateKeyboardAction(int actionCode, int& outVkCode, bool& outIsExtended);

	public:     
        static void GenerateRawMouseButton(int actionCode, bool press);
        static void GenerateRawKey(int vkCode, bool press, bool isExtended);
        static void SendActionDelta(int deltaX, int deltaY);
    };
}