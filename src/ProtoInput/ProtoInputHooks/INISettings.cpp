#include "INISettings.h"
#include <windows.h>
#include <cstdio>

namespace Proto
{
    char customWindowName[128] = "";
    char customClassName[128] = "";
    bool fixWindowFocus = false;

    void LoadConfig()
    {
        char path[MAX_PATH];
        if (GetModuleFileNameA(NULL, path, sizeof(path)))
        {
            char* lastSlash = strrchr(path, '\\');
            if (lastSlash) *lastSlash = '\0';

            strcat(path, "\\ProtoInput.ini");

            fixWindowFocus = GetPrivateProfileIntA("General", "FixWindowFocus", 0, path) != 0;

            GetPrivateProfileStringA("FindWindow", "WindowName", "", customWindowName, sizeof(customWindowName), path);
            GetPrivateProfileStringA("FindWindow", "ClassName", "", customClassName, sizeof(customClassName), path);
        }
    }
}