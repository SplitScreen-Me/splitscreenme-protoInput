#include "GtoMnK_RawInput.h"
#include "GtoMnK_RawInputHooks.h"
#include "HwndSelector.h"
#include "RawInput.h"

//copied from:https://github.com/SAM1430B/GtoMnK

namespace ScreenshotInput { 

        void RawInput::GenerateRawKey(int vkCode, bool press, bool isExtended) {
            if (vkCode == 0) return;

            RAWINPUT ri = {};
            ri.header.dwType = RIM_TYPEKEYBOARD;
            ri.header.hDevice = NULL;

            UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
            ri.data.keyboard.MakeCode = scanCode;
            ri.data.keyboard.Message = press ? WM_KEYDOWN : WM_KEYUP;
            ri.data.keyboard.VKey = vkCode;
            ri.data.keyboard.Flags = press ? RI_KEY_MAKE : RI_KEY_BREAK;

            if (isExtended) {
                ri.data.keyboard.Flags |= RI_KEY_E0;
            }

            Proto::RawInput::InjectFakeRawInput(ri);
        }

        void RawInput::GenerateRawMouseButton(int actionCode, bool press) {
            RAWINPUT ri = {};
            ri.header.dwType = RIM_TYPEMOUSE;
            ri.header.hDevice = NULL;
            if (actionCode == -8) { // Left Double
                GenerateRawMouseButton(-1, true); GenerateRawMouseButton(-1, false);
                GenerateRawMouseButton(-1, press); return;
            }
            if (actionCode == -9) { // Right Double
                GenerateRawMouseButton(-2, true); GenerateRawMouseButton(-2, false);
                GenerateRawMouseButton(-2, press); return;
            }
            if (actionCode == -10) { // Middle Double
                GenerateRawMouseButton(-3, true); GenerateRawMouseButton(-3, false);
                GenerateRawMouseButton(-3, press); return;
            }
            if (actionCode == -11) { // X1 Double
                GenerateRawMouseButton(-4, true); GenerateRawMouseButton(-4, false);
                GenerateRawMouseButton(-4, press); return;
            }
            if (actionCode == -12) { // X2 Double
                GenerateRawMouseButton(-5, true); GenerateRawMouseButton(-5, false);
                GenerateRawMouseButton(-5, press); return;
            }
            switch (actionCode) {
            case -1: ri.data.mouse.usButtonFlags = press ? RI_MOUSE_LEFT_BUTTON_DOWN : RI_MOUSE_LEFT_BUTTON_UP; break;
            case -2: ri.data.mouse.usButtonFlags = press ? RI_MOUSE_RIGHT_BUTTON_DOWN : RI_MOUSE_RIGHT_BUTTON_UP; break;
            case -3: ri.data.mouse.usButtonFlags = press ? RI_MOUSE_MIDDLE_BUTTON_DOWN : RI_MOUSE_MIDDLE_BUTTON_UP; break;
            case -4: ri.data.mouse.usButtonFlags = press ? RI_MOUSE_BUTTON_4_DOWN : RI_MOUSE_BUTTON_4_UP; break;
            case -5: ri.data.mouse.usButtonFlags = press ? RI_MOUSE_BUTTON_5_DOWN : RI_MOUSE_BUTTON_5_UP; break;
            case -6: if (press) ri.data.mouse.usButtonFlags = RI_MOUSE_WHEEL; ri.data.mouse.usButtonData = WHEEL_DELTA; break;
            case -7: if (press) ri.data.mouse.usButtonFlags = RI_MOUSE_WHEEL; ri.data.mouse.usButtonData = -WHEEL_DELTA; break;
            }
            Proto::RawInput::InjectFakeRawInput(ri);
        }

        // For keyboard actions for both methods
        void RawInput::TranslateKeyboardAction(int actionCode, int& outVkCode, bool& outIsExtended) {
            outVkCode = 0;
            outIsExtended = false;

            // Special & Modifier Keys
            if (actionCode == 1) outVkCode = VK_ESCAPE;
            if (actionCode == 2) outVkCode = VK_RETURN;      // Main Enter Key
            if (actionCode == 3) outVkCode = VK_TAB;
            if (actionCode == 4) outVkCode = VK_SHIFT;       // Generic Shift
            if (actionCode == 5) outVkCode = VK_LSHIFT;      // Left Shift
            if (actionCode == 6) outVkCode = VK_RSHIFT;      // Right Shift
            if (actionCode == 7) outVkCode = VK_CONTROL;     // Generic Control
            if (actionCode == 8) outVkCode = VK_LCONTROL;    // Left Control
            if (actionCode == 9) outVkCode = VK_RCONTROL;    // Right Control
            if (actionCode == 10) outVkCode = VK_MENU;       // Generic Alt
            if (actionCode == 11) outVkCode = VK_LMENU;      // Left Alt
            if (actionCode == 12) outVkCode = VK_RMENU;      // Right Alt
            if (actionCode == 13) outVkCode = VK_SPACE;
            if (actionCode == 14) outVkCode = VK_UP;         // Arrow Up
            if (actionCode == 15) outVkCode = VK_DOWN;       // Arrow Down
            if (actionCode == 16) outVkCode = VK_LEFT;       // Arrow Left
            if (actionCode == 17) outVkCode = VK_RIGHT;      // Arrow Right
            if (actionCode == 18) outVkCode = VK_BACK;       // Backspace
            if (actionCode == 19) outVkCode = VK_DELETE;
            if (actionCode == 20) outVkCode = VK_INSERT;
            if (actionCode == 21) outVkCode = VK_END;
            if (actionCode == 22) outVkCode = VK_HOME;
            if (actionCode == 23) outVkCode = VK_PRIOR;      // Page Up
            if (actionCode == 24) outVkCode = VK_NEXT;       // Page Down
            // Alphabet
            if (actionCode >= 25 && actionCode <= 50) outVkCode = 'A' + (actionCode - 25);
            // Top Row Numbers
            if (actionCode >= 51 && actionCode <= 60) outVkCode = '0' + (actionCode - 51);
            // F-Keys
            if (actionCode >= 61 && actionCode <= 72) outVkCode = VK_F1 + (actionCode - 61);
            // Numpad Numbers & Operators
            if (actionCode >= 73 && actionCode <= 82) outVkCode = VK_NUMPAD0 + (actionCode - 73);
            if (actionCode == 83) outVkCode = VK_ADD;
            if (actionCode == 84) outVkCode = VK_SUBTRACT;
            if (actionCode == 85) outVkCode = VK_MULTIPLY;
            if (actionCode == 86) outVkCode = VK_DIVIDE;
            if (actionCode == 87) outVkCode = VK_DECIMAL;
            if (actionCode == 88) outVkCode = VK_RETURN;     // Numpad Enter
            // Numpad Navigation (when NumLock is OFF)
            if (actionCode == 91) outVkCode = VK_INSERT;     // Numpad 0
            if (actionCode == 92) outVkCode = VK_END;        // Numpad 1
            if (actionCode == 93) outVkCode = VK_DOWN;       // Numpad 2
            if (actionCode == 94) outVkCode = VK_NEXT;       // Numpad 3
            if (actionCode == 95) outVkCode = VK_LEFT;       // Numpad 4
            if (actionCode == 96) outVkCode = VK_RIGHT;      // Numpad 6
            if (actionCode == 97) outVkCode = VK_HOME;       // Numpad 7
            if (actionCode == 98) outVkCode = VK_UP;         // Numpad 8
            if (actionCode == 99) outVkCode = VK_PRIOR;      // Numpad 9
            if (actionCode == 100) outVkCode = VK_DELETE;    // Numpad .
            // Lock Keys
            if (actionCode == 101) outVkCode = VK_CAPITAL;   // Caps Lock
            if (actionCode == 102) outVkCode = VK_NUMLOCK;   // Num Lock
            if (actionCode == 103) outVkCode = VK_SCROLL;    // Scroll Lock
            // Symbols
            if (actionCode == 104) outVkCode = VK_OEM_1;      // ;  (Semicolon)
            if (actionCode == 105) outVkCode = VK_OEM_PLUS;   // =  (Plus/Equal)
            if (actionCode == 106) outVkCode = VK_OEM_COMMA;  // ,  (Comma)
            if (actionCode == 107) outVkCode = VK_OEM_MINUS;  // -  (Minus)
            if (actionCode == 108) outVkCode = VK_OEM_PERIOD; // .  (Period)
            if (actionCode == 109) outVkCode = VK_OEM_2;      // /  (Forward Slash)
            if (actionCode == 110) outVkCode = VK_OEM_3;      // `  (Grave Accent)
            if (actionCode == 111) outVkCode = VK_OEM_4;      // [  (Left Bracket)
            if (actionCode == 112) outVkCode = VK_OEM_5;      // \  (Backslash)
            if (actionCode == 113) outVkCode = VK_OEM_6;      // ]  (Right Bracket)
            if (actionCode == 114) outVkCode = VK_OEM_7;      // '  (Apostrophe)
            // Extended Keys
            if ((actionCode >= 14 && actionCode <= 17) || // Dedicated Arrow Keys
                (actionCode >= 19 && actionCode <= 24) || // Insert, Del, Home, End, PgUp, PgDn
                actionCode == 9 ||  // Right Control
                actionCode == 12 || // Right Alt
                actionCode == 88 || // Numpad Enter
                actionCode == 102)
            {
                outIsExtended = true;
            }
        }
        void RawInput::SendActionDelta(int deltaX, int deltaY) 
        {
            RAWINPUT ri = {};
            ri.header.dwType = RIM_TYPEMOUSE;
            ri.header.hDevice = NULL;
            ri.data.mouse.usFlags = MOUSE_MOVE_RELATIVE;
            ri.data.mouse.lLastX = deltaX;
            ri.data.mouse.lLastY = deltaY;
            Proto::RawInput::InjectFakeRawInput(ri);
        }
    
}