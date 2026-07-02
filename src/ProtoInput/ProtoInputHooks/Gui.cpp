#include "Gui.h"
#include "imgui.h"
#include <cstdio>
#include "RawInput.h"
#include <iostream>
#include "Windows.h"
#include "HookManager.h"
#include <algorithm>
#include "MessageFilterHook.h"
#include "MessageList.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "FocusMessageLoop.h"
#include "StateInfo.h"
#include "FakeCursor.h"
#include "TranslateXtoMKB.h" 
#include "ScanThread.h" 
#include "GtoMnK_RawInput.h" 
#include "XinputHook.h" 
#include "WindowMsgHook.h" 


namespace Proto
{
intptr_t ConsoleHwnd;

bool PointerInMouseold = false;

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ShowTooltip(const char* text)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

template<typename T>
void HandleSelectableDualList(std::vector<T>& selected, std::vector<T>& deselected)
{
    const char* prefix = "  ";
	
    std::sort(selected.begin(), selected.end());
    std::sort(deselected.begin(), deselected.end());
	
    std::vector<T> toAddToB{};
    std::vector<std::vector<T>::const_iterator> removeA{};
    std::vector<std::vector<T>::const_iterator> removeB{};

    for (size_t i = 0; i < selected.size(); ++i)
    {
        char buf[32];
        sprintf(buf, "%sHandle %d", prefix, (intptr_t)selected[i]);
        if (ImGui::Selectable(buf, true))
        {
            toAddToB.push_back(selected[i]);
            removeA.push_back(selected.begin() + i);
        }
    }
	
    for (size_t i = 0; i < deselected.size(); ++i)
    {
        char buf[32];
        sprintf(buf, "%sHandle %d", prefix, (intptr_t)deselected[i]);
        if (ImGui::Selectable(buf, false))
        {
            selected.push_back(deselected[i]);
            removeB.push_back(deselected.begin() + i);
        }
    }

    for (const auto x : toAddToB)
        deselected.push_back(x);
    for (const auto x : removeA)
        selected.erase(x);
    for (const auto x : removeB)
        deselected.erase(x);
}
std::string VkToKeyName(int vk)
{

    UINT scan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    UINT flags = scan << 16;

    // Add extended-key flag when needed
    switch (vk)
    {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR: // Page Up
    case VK_NEXT:  // Page Down
    case VK_RCONTROL:
    case VK_RMENU: // Right Alt
    case VK_DIVIDE:
    case VK_NUMLOCK:
        flags |= (1 << 24);
        break;
    }

    char name[64] = { 0 };
    GetKeyNameTextA(flags, name, sizeof(name));
    return std::string(name);
}
bool mappingrefreshed = false;
USHORT X_A;
USHORT X_B;
USHORT X_X;
USHORT X_Y;

USHORT X_RS;
USHORT X_LS;
USHORT X_right;
USHORT X_left;
USHORT X_up;
USHORT X_down;

USHORT X_stickLpress;
USHORT X_stickRpress;
USHORT X_stickright;
USHORT X_stickleft;
USHORT X_stickup;
USHORT X_stickdown;

USHORT X_option;
USHORT X_start;
//bool 



int lastVKkey;
void XTranslatefreshmapping(bool read) {
    if (read) {
        //collision danger if remote read each frame
        X_A = ScreenshotInput::TranslateXtoMKB::Amapping;
        X_B = ScreenshotInput::TranslateXtoMKB::Bmapping;
        X_X = ScreenshotInput::TranslateXtoMKB::Xmapping;
        X_Y = ScreenshotInput::TranslateXtoMKB::Ymapping;

        X_RS = ScreenshotInput::TranslateXtoMKB::RSmapping;
        X_LS = ScreenshotInput::TranslateXtoMKB::LSmapping;
        X_right = ScreenshotInput::TranslateXtoMKB::rightmapping;
        X_left = ScreenshotInput::TranslateXtoMKB::leftmapping;
        X_up = ScreenshotInput::TranslateXtoMKB::upmapping;
        X_down = ScreenshotInput::TranslateXtoMKB::downmapping;

        X_stickRpress = ScreenshotInput::TranslateXtoMKB::stickRpressmapping;
        X_stickLpress = ScreenshotInput::TranslateXtoMKB::stickLpressmapping;
        X_stickright = ScreenshotInput::TranslateXtoMKB::stickrightmapping;
        X_stickleft = ScreenshotInput::TranslateXtoMKB::stickleftmapping;
        X_stickup = ScreenshotInput::TranslateXtoMKB::stickupmapping;
        X_stickdown = ScreenshotInput::TranslateXtoMKB::stickdownmapping;

        X_option = ScreenshotInput::TranslateXtoMKB::optionmapping;
        X_start = ScreenshotInput::TranslateXtoMKB::startmapping;
        X_stickdown = ScreenshotInput::TranslateXtoMKB::stickdownmapping;
    }//ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);

}
void GetVK()
{
    BYTE keys[256];
    GetKeyboardState(keys);
    for (int vk = 0; vk < 256; vk++)
    {
        if (keys[vk] & 0x80)
        {
            lastVKkey = vk;
            return;
        }
    }
}

void XTranslateMenu()
{
    if (!mappingrefreshed)
        XTranslatefreshmapping(true);
    
    ImGui::SliderInt("Sensitivity flat", (int*)&ScreenshotInput::TranslateXtoMKB::Sens, 1, 40, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Separator();
    ImGui::SliderInt("Sensitivity exponential", (int*)&ScreenshotInput::TranslateXtoMKB::Sensmult, 1, 20, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Separator();
    ImGui::SliderInt("Deadzone", (int*)&ScreenshotInput::TranslateXtoMKB::Deadzone, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Separator();
    {
        const auto XAString = VkToKeyName(X_A);
        ImGui::TextWrapped("A is mapped to: %s", (XAString.c_str()));

        static bool waitingKeyPressA = false;

        if (waitingKeyPressA) 
        {
            //PushDisabled();
            GetVK();
            ImGui::Button("Press Keyboard button...##A"); //these need unique IDs or text
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressA = false;
                X_A = lastVKkey;
			    ScreenshotInput::TranslateXtoMKB::Amapping = X_A;
            }
        }
    
        else if (ImGui::Button("Click to change##A1"))//these need unique IDs or text
        {
            waitingKeyPressA = true;
            lastVKkey = -1;
        }
    }
	ImGui::Separator();
    {
        const auto XBString = VkToKeyName(X_B);
        ImGui::TextWrapped("B is mapped to: %s", (XBString.c_str()));

        static bool waitingKeyPressB = false;
        if (waitingKeyPressB)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##B"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressB = false;
                X_B = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::Bmapping = X_B;
            }
        }
        else if (ImGui::Button("Click to change##B1"))//these need unique IDs or text
        {
            waitingKeyPressB = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XXString = VkToKeyName(X_X);
        ImGui::TextWrapped("X is mapped to: %s", (XXString.c_str()));

        static bool waitingKeyPressX = false;
        if (waitingKeyPressX)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##X"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressX = false;
                X_X = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::Xmapping = X_X;
            }
        }
        else if (ImGui::Button("Click to change##X1"))//these need unique IDs or text
        {
            waitingKeyPressX = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XYString = VkToKeyName(X_Y);
        ImGui::TextWrapped("Y is mapped to: %s", (XYString.c_str()));

        static bool waitingKeyPressY = false;
        if (waitingKeyPressY)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##Y"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressY = false;
                X_Y = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::Ymapping = X_Y;
            }
        }
        else if (ImGui::Button("Click to change##Y1"))//these need unique IDs or text
        {
            waitingKeyPressY = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XRSString = VkToKeyName(X_RS);
        ImGui::TextWrapped("Right Shoulder is mapped to: %s", (XRSString.c_str()));

        static bool waitingKeyPressRS = false;
        if (waitingKeyPressRS)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##RS"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressRS = false;
                X_RS = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::RSmapping = X_RS;
            }
        }
        else if (ImGui::Button("Click to change##RS1"))//these need unique IDs or text
        {
            waitingKeyPressRS = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XLSString = VkToKeyName(X_LS);
        ImGui::TextWrapped("Left Shoulder is mapped to: %s", (XLSString.c_str()));

        static bool waitingKeyPressLS = false;
        if (waitingKeyPressLS)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##LS"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressLS = false;
                X_LS = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::LSmapping = X_LS;
            }
        }
        else if (ImGui::Button("Click to change##LS1"))//these need unique IDs or text
        {
            waitingKeyPressLS = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XrightString = VkToKeyName(X_right);
        ImGui::TextWrapped("DPAD right is mapped to: %s", (XrightString.c_str()));

        static bool waitingKeyPressright = false;
        if (waitingKeyPressright)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##DR"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressright = false;
                X_right = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::rightmapping = X_right;
            }
        }
        else if (ImGui::Button("Click to change##DR1"))//these need unique IDs or text
        {
            waitingKeyPressright = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XleftString = VkToKeyName(X_left);
        ImGui::TextWrapped("DPAD left is mapped to: %s", (XleftString.c_str()));

        static bool waitingKeyPressleft = false;
        if (waitingKeyPressleft)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##DL"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressleft = false;
                X_left = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::leftmapping = X_left;
            }
        }
        else if (ImGui::Button("Click to change##DL1"))//these need unique IDs or text
        {
            waitingKeyPressleft = true;
            lastVKkey = -1;
        }
    }

    ImGui::Separator();
    {
        const auto XupString = VkToKeyName(X_up);
        ImGui::TextWrapped("DPAD up is mapped to: %s", (XupString.c_str()));

        static bool waitingKeyPressup = false;
        if (waitingKeyPressup)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##DU"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressup = false;
                X_up = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::upmapping = X_up;
            }
        }
        else if (ImGui::Button("Click to change##DU1"))//these need unique IDs or text
        {
            waitingKeyPressup = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XdownString = VkToKeyName(X_down);
        ImGui::TextWrapped("DPAD down is mapped to: %s", (XdownString.c_str()));

        static bool waitingKeyPressdown = false;
        if (waitingKeyPressdown)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##DD"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressdown = false;
                X_down = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::downmapping = X_down;
            }
        }
        else if (ImGui::Button("Click to change##DD1"))//these need unique IDs or text
        {
            waitingKeyPressdown = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickRpressString = VkToKeyName(X_stickRpress);
        ImGui::TextWrapped("Right stick press is mapped to: %s", (XstickRpressString.c_str()));

        static bool waitingKeyPressstickRpress = false;
        if (waitingKeyPressstickRpress)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##RSP"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickRpress = false;
                X_stickRpress = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickRpressmapping = X_stickRpress;
            }
        }
        else if (ImGui::Button("Click to change##RSP1"))//these need unique IDs or text
        {
            waitingKeyPressstickRpress = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickLpressString = VkToKeyName(X_stickLpress);
        ImGui::TextWrapped("left stick press is mapped to: %s", (XstickLpressString.c_str()));

        static bool waitingKeyPressstickLpress = false;
        if (waitingKeyPressstickLpress)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##LSP"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickLpress = false;
                X_stickLpress = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickLpressmapping = X_stickLpress;
            }
        }
        else if (ImGui::Button("Click to change##LSP1"))//these need unique IDs or text
        {
            waitingKeyPressstickLpress = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickrightString = VkToKeyName(X_stickright);
        ImGui::TextWrapped("Stick right axis is mapped to: %s", (XstickrightString.c_str()));

        static bool waitingKeyPressstickright = false;
        if (waitingKeyPressstickright)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##SRA"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickright = false;
                X_stickright = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickrightmapping = X_stickright;
            }
        }
        else if (ImGui::Button("Click to change##SRA1"))//these need unique IDs or text
        {
            waitingKeyPressstickright = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickleftString = VkToKeyName(X_stickleft);
        ImGui::TextWrapped("Stick left axis is mapped to: %s", (XstickleftString.c_str()));

        static bool waitingKeyPressstickleft = false;
        if (waitingKeyPressstickleft)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##SLA"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickleft = false;
                X_stickleft = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickleftmapping = X_stickleft;
            }
        }
        else if (ImGui::Button("Click to change##SLA1"))//these need unique IDs or text
        {
            waitingKeyPressstickleft = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickupString = VkToKeyName(X_stickup);
        ImGui::TextWrapped("Stick up axis is mapped to: %s", (XstickupString.c_str()));

        static bool waitingKeyPressstickup = false;
        if (waitingKeyPressstickup)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##SUA"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickup = false;
                X_stickup = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickupmapping = X_stickup;
            }
        }
        else if (ImGui::Button("Click to change##SUA1"))//these need unique IDs or text
        {
            waitingKeyPressstickup = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstickdownString = VkToKeyName(X_stickdown);
        ImGui::TextWrapped("Stick down axis is mapped to: %s", (XstickdownString.c_str()));

        static bool waitingKeyPressstickdown = false;
        if (waitingKeyPressstickdown)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##SDA"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstickdown = false;
                X_stickdown = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::stickdownmapping = X_stickdown;
            }
        }
        else if (ImGui::Button("Click to change##SDA1"))//these need unique IDs or text
        {
            waitingKeyPressstickdown = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XoptionString = VkToKeyName(X_option);
        ImGui::TextWrapped("Options button is mapped to: %s", (XoptionString.c_str()));

        static bool waitingKeyPressoption = false;
        if (waitingKeyPressoption)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##OPT"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressoption = false;
                X_option = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::optionmapping = X_option;
            }
        }
        else if (ImGui::Button("Click to change##OPT1"))//these need unique IDs or text
        {
            waitingKeyPressoption = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator();
    {
        const auto XstartString = VkToKeyName(X_start);
        ImGui::TextWrapped("Start button is mapped to: %s", (XstartString.c_str()));

        static bool waitingKeyPressstart = false;
        if (waitingKeyPressstart)
        {
            //PushDisabled();
            ImGui::Button("Press Keyboard button...##STA"); //these need unique IDs or text
            GetVK();
            //PopDisabled();
           // Sleep(100);
            if (lastVKkey != -1)
            {
                waitingKeyPressstart = false;
                X_start = lastVKkey;
                ScreenshotInput::TranslateXtoMKB::startmapping = X_start;
            }
        }
        else if (ImGui::Button("Click to change##STA1"))//these need unique IDs or text
        {
            waitingKeyPressstart = true;
            lastVKkey = -1;
        }
    }
    ImGui::Separator(); //no idea if this may crash. suppose it is not safe
    ImGui::Checkbox("Lefthanded Stick. moves mouse with left stick and button map on right stick. or opposite if disabled", &ScreenshotInput::TranslateXtoMKB::lefthanded); //
    ImGui::Separator();
    ImGui::Separator();
    if (RawInput::TranslateXinputtoMKB)
    { 
        ImGui::Checkbox("Shoulder Swap BMPs", &ScreenshotInput::ScanThread::ShoulderNextBMP); //
        ImGui::Separator();
        ImGui::Text("Input actions for Scanoption. 0 is move+click. 1 is only move. 2 is only click");
        ImGui::SliderInt("A coordinate", (int*)&ScreenshotInput::ScanThread::scanAtype, 0, 2, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("B coordinate", (int*)&ScreenshotInput::ScanThread::scanBtype, 0, 2, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("X coordinate", (int*)&ScreenshotInput::ScanThread::scanXtype, 0, 2, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("Y coordinate", (int*)&ScreenshotInput::ScanThread::scanYtype, 0, 2, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Separator();
        ImGui::Text("Save BMP mode. buttons XYAB will save a bmp on press at fake cursor coordinate when enabled. This option also force ScanOption to deactivate");
        ImGui::Checkbox("Save BMP mode:", &ScreenshotInput::TranslateXtoMKB::SaveBmps); //
        ImGui::Separator();
        ImGui::Text("Scanoption will need a restart to discover new bmps.");
        ImGui::Checkbox("ScanOption:", &ScreenshotInput::ScanThread::scanoption); //
        ImGui::Separator();
        if (!ScreenshotInput::ScanThread::scanoption)
		    ScreenshotInput::ScanThread::scanloop = false;
    }
}
void HooksMenu()
{
    const auto& hooks = HookManager::GetHooks();
    
    static int selected = 0;
    
    ImGui::BeginChild("left pane", ImVec2(180, 0), true);

    for (int i = 0; i < hooks.size(); i++)
    {
        const auto& hook = hooks[i];
        
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_Text, 
                              hook->IsInstalled() ? 
								(ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f) :
								(ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f)
        );

        if (ImGui::Selectable(hook->GetHookName(), selected == i))
            selected = i;

        ImGui::PopStyleColor(1);
        ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    if (!hooks.empty())
    {
        const auto& hook = hooks[selected];
    	
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        ImGui::TextWrapped("%s Hook: %s", hook->GetHookName(), hook->IsInstalled() ? "Enabled" : "Disabled");

    	// ImGui::SameLine();
        if (ImGui::Button("Enable") && !hook->IsInstalled())
        {
            hook->Install();
        }

        ImGui::SameLine();

        if (ImGui::Button("Disable") && hook->IsInstalled())
        {
            hook->Uninstall();
        }
    	
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Description"))
            {
                ImGui::TextWrapped(hook->GetHookDescription());
                ImGui::EndTabItem();
            }
            
            if (hook->HasGuiStatus() && ImGui::BeginTabItem("Details"))
            {
                hook->ShowGuiStatus();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    	
        ImGui::EndChild();
    	
        ImGui::EndGroup();
    }
}

void RawInputMenu()
{
    bool externalFreezeInput = RawInput::rawInputState.externalFreezeInput;
    ImGui::Checkbox("External freeze fake input", &externalFreezeInput);
    ShowTooltip("Stops sending fake/virtual input to the game. This can only be set from the API, for scripting tools. ");
	
    ImGui::Checkbox("Freeze fake input", &RawInput::rawInputState.freezeInput);
    ShowTooltip("Stops sending fake/virtual input to the game. Use this when debugging/scripting. ");
	
    ImGui::Checkbox("Freeze fake input while GUI opened", &RawInput::rawInputState.freezeInputWhileGuiOpened);
    ShowTooltip("Stops sending fake/virtual input to the game while the GUI is opened. "
					"Use this when debugging/scripting so you don't accidentally control the game changing the GUI settings. ");

    ImGui::Separator();

    //this enough to prevent both en enabled?
    if (!XinputHook::TranslateMKBtoXinput)
        ImGui::Checkbox("TranslateXtoMKB", &RawInput::TranslateXinputtoMKB);
    ImGui::Separator();
    if (!RawInput::TranslateXinputtoMKB)
        ImGui::Checkbox("TranslateMKBtoX", &XinputHook::TranslateMKBtoXinput);
    ImGui::Separator();
    if (RawInput::TranslateXinputtoMKB && XinputHook::TranslateMKBtoXinput)
        RawInput::TranslateXinputtoMKB = false;
    RawInput::TranslateXinputtoMKB2 = RawInput::TranslateXinputtoMKB;

    bool showFakeCursor = FakeCursor::IsDrawingEnabled();
    if (ImGui::Checkbox("Draw fake cursor", &showFakeCursor))
    {
        FakeCursor::EnableDisableFakeCursor(showFakeCursor);
    }

    bool ignoreMouseBounds = FakeMouseKeyboard::GetMouseState().ignoreMouseBounds;
    if (ImGui::Checkbox("Ignore mouse bounds", &ignoreMouseBounds))
    {
        FakeMouseKeyboard::SetIgnoreMouseBounds(ignoreMouseBounds);
    }
    ShowTooltip("Allows the fake cursor to go beyond the window boundaries. Can fix a problem in some SDL2 games. ");

    bool extendMouseBounds = FakeMouseKeyboard::GetMouseState().extendMouseBounds;
    if (ImGui::Checkbox("Extend mouse bounds", &extendMouseBounds))
    {
        FakeMouseKeyboard::SetExtendMouseBounds(extendMouseBounds);
    }
    ShowTooltip("Allows the fake cursor to go slightly beyond the window boundaries. Useful if there is a mouse offset. ");

    ImGui::Checkbox("Toggle visibility shortcut", &FakeCursor::GetToggleVisilbityShorcutEnabled());
    ShowTooltip("The Set Cursor Visibility hook doesn't work on some games, so a keyboard shortcut to toggle visibility can be used instead. ");

    ImGui::InputInt("Toggle visibility keyboard VKey", (int*)&FakeCursor::GetToggleVisibilityVkey(), 1, 100);
	
    ImGui::Separator();
    
    ImGui::Checkbox("Translate mouse messages to Pointermessages", &RawInput::PointerInMouse);

    if (PointerInMouseold != RawInput::PointerInMouse)
        WindowMsgHook::PointerInMouse(RawInput::PointerInMouse);
    PointerInMouseold = RawInput::PointerInMouse;
    ImGui::Checkbox("Send mouse movement messages", &RawInput::rawInputState.sendMouseMoveMessages);
    ImGui::Checkbox("Send mouse button messages", &RawInput::rawInputState.sendMouseButtonMessages);
    ImGui::Checkbox("Send mouse wheel messages", &RawInput::rawInputState.sendMouseWheelMessages);
    ImGui::Checkbox("Send keyboard button messages", &RawInput::rawInputState.sendKeyboardPressMessages);
    ImGui::Checkbox("Send mouse double click messages", &RawInput::rawInputState.sendMouseDblClkMessages);
    ImGui::Checkbox("Send Messages to Subwindows", &RawInput::MessageAllWindows);

    ImGui::Separator();
	


    if (RawInput::TranslateXinputtoMKB) //TranslateXisenabled
    {
		
        ImGui::TextWrapped("XinputtoMKB ControllerID");
        ImGui::TextWrapped("0 is first controller");
        ImGui::SliderInt("XinputtoMKB ControllerID", (int*)&ScreenshotInput::TranslateXtoMKB::controllerID, 0, 16, "%d", ImGuiSliderFlags_AlwaysClamp);
    }
    else
    {
        if (ImGui::TreeNode("Selected mouse devices"))
        {
            HandleSelectableDualList(RawInput::rawInputState.selectedMouseHandles, RawInput::rawInputState.deselectedMouseHandles);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Selected keyboard devices"))
        {
            HandleSelectableDualList(RawInput::rawInputState.selectedKeyboardHandles, RawInput::rawInputState.deselectedKeyboardHandles);

            ImGui::TreePop();
        }
        if (ImGui::Button("Refresh devices"))
        {
            RawInput::RefreshDevices();
        }
    }

}

void ControlsMenu()
{ 
	if (ImGui::Button("Hide GUI"))
	{
        SetWindowVisible(false);
	}
    ShowTooltip("Press right control + right alt + 1/2/3/4/... to open the GUI for instance 1/2/3/4/...");

	if (ImGui::Button("Toggle console"))
	{
        ToggleConsole();
	}

    ImGui::Checkbox("Lock input shortcut", &RawInput::lockInputToggleEnabled);
    ShowTooltip("If this is enabled, pressing the Home key will lock the keyboard and mouse input. "
					"Input lock should only be enabled from inside the hooks for debugging/scripting purposes. "
					"WARNING: Make sure to disable the lock before you close the game or explorer.exe will remain frozen. ");
}

void InputStatusMenu()
{
    const auto& mouseState = FakeMouseKeyboard::GetMouseState();
    ImGui::TextWrapped("Fake mouse position (%d, %d)", mouseState.x, mouseState.y);
    ImGui::TextWrapped("Window dimensions: (%d, %d)", HwndSelector::windowWidth, HwndSelector::windowHeight);
}

void FocusMessageLoopMenu()
{
    ImGui::TextWrapped("Focus message loop: %s", FocusMessageLoop::running ? "Running" : "Not running");

    if (FocusMessageLoop::running && ImGui::Button("Pause"))
        FocusMessageLoop::PauseMessageLoop();
    else if (!FocusMessageLoop::running && ImGui::Button("Resume"))
        FocusMessageLoop::StartMessageLoop();

    ImGui::InputInt("Interval (ms)", &FocusMessageLoop::sleepMilliseconds, 1, 100);
    if (FocusMessageLoop::sleepMilliseconds < 0)
        FocusMessageLoop::sleepMilliseconds = 0;
	
    ImGui::TextWrapped("Messages to send:");
		
    ImGui::Checkbox("WM_ACTIVATE", &FocusMessageLoop::messagesToSend.wm_activate);
    ImGui::Checkbox("WM_NCACTIVATE", &FocusMessageLoop::messagesToSend.wm_ncactivate);
    ImGui::Checkbox("WM_ACTIVATEAPP", &FocusMessageLoop::messagesToSend.wm_activateapp);
    ImGui::Checkbox("WM_SETFOCUS", &FocusMessageLoop::messagesToSend.wm_setfocus);
    ImGui::Checkbox("WM_MOUSEACTIVATE", &FocusMessageLoop::messagesToSend.wm_mouseactivate);
}

static void InfoMenu()
{
    ImGui::TextWrapped("Instance index %d", StateInfo::info.instanceIndex);
}

void RenderImgui()
{
    // ImGui::ShowDemoWindow();
    // return;
    const auto displaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, displaySize.y), ImGuiCond_Always);

    if (ImGui::Begin("Main", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        //ImGuiWindowFlags_NoBackground |
        // ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus
    ))
    {

        const ImVec2 mainWindowSize = ImGui::GetWindowSize();

        ImGui::SetNextWindowSizeConstraints(ImVec2(200, displaySize.y), ImVec2(displaySize.x - 200, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 0.7f, 0.0f), ImGuiCond_Once);

        ImGui::Begin("Hooks/Filter", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        const auto hooksWindowSize = ImVec2(ImGui::GetWindowSize().x, mainWindowSize.y);
        ImGui::SetWindowSize(hooksWindowSize);

        if (ImGui::BeginTabBar("Tabs"))
        {
            if (ImGui::BeginTabItem("Hooks"))
            {
                HooksMenu();
                ImGui::EndTabItem();
            }
            if (RawInput::TranslateXinputtoMKB || XinputHook::TranslateMKBtoXinput)
            {
                if (ImGui::BeginTabItem("Translation options"))
                {
                    XTranslateMenu();
                    ImGui::EndTabItem();
                }
            }

            if (ImGui::BeginTabItem("Message filter"))
            {
                if (ImGui::BeginTabBar("Filter tabs"))
                {
                    if (ImGui::BeginTabItem("Modify"))
                    {
                        if (!HookManager::IsInstalled(ProtoHookIDs::MessageFilterHookID))
                        {
                            ImGui::PushID(1337);
                            ImGui::PushStyleColor(ImGuiCol_Text,
                                (ImVec4)ImColor::HSV(35.0f / 255.0f, 0.9f, 0.9f)
                            );
                            ImGui::TextWrapped("Warning: Message Filter hook is disabled.\nMessage filtering/blocking will not work!");
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();
                        }

                        MessageFilterHook::FilterGui();

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Block"))
                    {
                        if (!HookManager::IsInstalled(ProtoHookIDs::MessageFilterHookID))
                        {
                            ImGui::PushID(1337);
                            ImGui::PushStyleColor(ImGuiCol_Text,
                                (ImVec4)ImColor::HSV(35.0f / 255.0f, 0.9f, 0.9f)
                            );
                            ImGui::TextWrapped("Warning: Message Filter hook is disabled.\nMessage filtering/blocking will not work!");
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();
                        }

                        MessageList::ShowUI();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();


        ImGui::SetNextWindowPos(ImVec2(hooksWindowSize.x, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, displaySize.y), ImVec2(displaySize.x - 200, displaySize.y));
        ImGui::SetNextWindowSize(ImVec2(displaySize.x - hooksWindowSize.x, 0.0f), ImGuiCond_Always);

        ImGui::Begin("Other", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        const auto otherWindowSize = ImVec2(ImGui::GetWindowSize().x, mainWindowSize.y);
        ImGui::SetWindowSize(otherWindowSize);
        const auto otherWindowPos = ImGui::GetWindowPos();

        if (ImGui::CollapsingHeader("Info", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
        {
            InfoMenu();
        }

        if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen)) // ImGuiTreeNodeFlags_Leaf
        {
            ControlsMenu();
        }

        if (ImGui::CollapsingHeader("Raw Input", ImGuiTreeNodeFlags_DefaultOpen))
        {
            RawInputMenu();
        }

        if (ImGui::CollapsingHeader("Input Status", ImGuiTreeNodeFlags_DefaultOpen))
        {
            InputStatusMenu();
        }

        if (ImGui::CollapsingHeader("Focus Message Loop", ImGuiTreeNodeFlags_DefaultOpen))
        {
            FocusMessageLoopMenu();
        }

        ImGui::End();

    }
    ImGui::End();

}
DWORD WINAPI GuiThread(LPVOID lpParameter)
{
    std::cout << "Starting gui thread\n";

    Proto::AddThreadToACL(GetCurrentThreadId());

    Proto::ShowGuiImpl();

    return 0;
}
void StartGUIThread()
{ 
    HANDLE hGuiThread = CreateThread(nullptr, 0,
        (LPTHREAD_START_ROUTINE)GuiThread, Proto::hmodule, CREATE_SUSPENDED, &Proto::GuiThreadID);

    ResumeThread(hGuiThread);

    if (hGuiThread != nullptr)
    CloseHandle(hGuiThread);
}
}
