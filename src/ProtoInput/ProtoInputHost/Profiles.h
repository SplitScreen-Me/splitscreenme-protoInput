#pragma once
#include <filesystem>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>
#include "protoloader.h"
#include "MessageList.h"

namespace ProtoHost
{

struct ProfileOption
{
	std::string name;
	std::string uiLabel;
	bool enabled = false;
	unsigned int id;
	
	ProfileOption() = default;
	
	template<class T = unsigned int>
	ProfileOption(std::string _name, bool _enabled, T _id = 0) : name(std::move(_name)), uiLabel(name), enabled(_enabled), id((unsigned int)_id) {}

	template<class T = unsigned int>
	ProfileOption(std::string _name, bool _enabled, std::string _uiLabel, T _id = 0) : name(std::move(_name)), uiLabel(std::move(_uiLabel)),
			enabled(_enabled), id((unsigned int)_id) {}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("name", name),
			cereal::make_nvp("enabled", enabled)
		);
	}
};

struct Profile //Profile::hooks::ProtoHookIDs::RenameHandlesHookID
{
	std::vector<ProfileOption> hooks
	{
		{ "Register Raw Input", true, "Register Raw Input", ProtoHookIDs::RegisterRawInputHookID }, //0
		{ "Get Raw Input Data", true, "Get Raw Input Data", ProtoHookIDs::GetRawInputDataHookID }, //1
		{ "Message Filter", true, "Message Filter", ProtoHookIDs::MessageFilterHookID }, //2
		{ "Get Cursor Position", true, "Get Cursor Position", ProtoHookIDs::GetCursorPosHookID },
		{ "Set Cursor Position", true, "Set Cursor Position", ProtoHookIDs::SetCursorPosHookID },
		{ "Get Key State", true, "Get Key State", ProtoHookIDs::GetKeyStateHookID },
		{ "Get Async Key State", true, "Get Async Key State", ProtoHookIDs::GetAsyncKeyStateHookID },
		{ "Get Keyboard State", true, "Get Keyboard State", ProtoHookIDs::GetKeyboardStateHookID },
		{ "Cursor Visibility", true, "Cursor Visibility", ProtoHookIDs::CursorVisibilityStateHookID },
		{ "Clip Cursor", true, "Clip Cursor", ProtoHookIDs::ClipCursorHookID },
		{ "Focus", true, "Focus", ProtoHookIDs::FocusHooksHookID },
		{ "Rename Handles", true, "Rename Handles", ProtoHookIDs::RenameHandlesHookID },
		{ "Block Raw Input", false, "Block Raw Input", ProtoHookIDs::BlockRawInputHookID },
		{ "Dinput Order", false, "Dinput Order", ProtoHookIDs::DinputOrderHookID },
		{ "Xinput", false, "Xinput", ProtoHookIDs::XinputHookID },
		{ "GetCursorInfo", false, "GetCursorInfo", ProtoHookIDs::GetCursorInfoHookID },
		{ "SetWindowsHookHook", false, "SetWindowsHookHook", ProtoHookIDs::SetWindowsHookHookID }
	};

	std::vector<ProfileOption> messageFilters
	{
		{ "Raw Input", true, "Raw Input", ProtoMessageFilterIDs::RawInputFilterID },
		{ "Mouse Move", true, "Mouse Move", ProtoMessageFilterIDs::MouseMoveFilterID },
		{ "Mouse Activate", true, "Mouse Activate", ProtoMessageFilterIDs::MouseActivateFilterID },
		{ "Window Activate", true, "Window Activate", ProtoMessageFilterIDs::WindowActivateFilterID },
		{ "Window Activate App", true, "Window Activate App", ProtoMessageFilterIDs::WindowActivateAppFilterID },
		{ "Mouse Wheel Filter", true, "Mouse Wheel Filter", ProtoMessageFilterIDs::MouseWheelFilterID },
		{ "Mouse Button", true, "Mouse Button", ProtoMessageFilterIDs::MouseButtonFilterID },
		{ "Keyboard Button", true, "Keyboard Button", ProtoMessageFilterIDs::KeyboardButtonFilterID },
	};

	bool dinputToXinputRedirection = false;
	bool useOpenXinput = false;

	//reregister devices to game after hooking
	bool Reregisterinput = false;
	bool PointerInMouse = false;

	bool TranslateMKBtoXinput = false;
	bool TranslateXinputtoMKB = false;

	bool XinputtoMKBstickinvert = false;
	bool ScanOption = false;
	bool Shoulderswappoints = false;
	bool XAstatic = false;
	bool XAclick = true;
	bool XAmove = true;
	bool XBstatic = false;
	bool XBclick = true;
	bool XBmove = true;
	bool XXstatic = false;
	bool XXclick = true;
	bool XXmove = true;
	bool XYstatic = false;
	bool XYclick = true;
	bool XYmove = true;
	int XinputtoMKBAkey = 0x52; //R
	int XinputtoMKBBkey = 0x47; //G
	int XinputtoMKBXkey = 0x45; //E
	int XinputtoMKBYkey = 0x43; //C
	int XinputtoMKBRSkey = 0x10;
	int XinputtoMKBLSkey = 0x20;
	int XinputtoMKBrightkey = 0x27;
	int XinputtoMKBleftkey = 0x25;
	int XinputtoMKBupkey = 0x26;
	int XinputtoMKBdownkey = 0x28;
	int XinputtoMKBstickR = 0x5A; //Z
	int XinputtoMKBstickL = 0x4D; //M
	int XinputtoMKBstickright = 0x41; //A
	int XinputtoMKBstickleft = 0x44; //D
	int XinputtoMKBstickup = 0x57; //W
	int XinputtoMKBstickdown = 0x53; //S
	int XinputtoMKBoption = 0x1B; //
	int XinputtoMKBstart = 0x0D;
	int XinputtoMKBsens = 15;
	int XinputtoMKBsensmult = 4;
	int XinputtoMKBDeadzone = 2;
	

	bool useFakeClipCursor = true;

	bool showCursorWhenImageUpdated = false;

	bool putMouseInsideWindow = true;
	
	bool drawFakeMouseCursor = true;
	bool drawFakeCursorFix = false;
	bool allowMouseOutOfBounds = false;
	bool extendMouseBounds = false;
	bool toggleFakeCursorVisibilityShortcut = false;
	bool sendMouseMovementMessages = true;
	bool sendMouseButtonMessages = true;
	bool sendMouseWheelMessages = true;
	bool sendKeyboardButtonMessages = true;
	bool sendMouseDblClkMessages = false;
	
	bool focusMessageLoop = true;
	bool focusLoopSendWM_ACTIVATE = true;
	bool focusLoopSendWM_NCACTIVATE = false;
	bool focusLoopSendWM_ACTIVATEAPP = false;
	bool focusLoopSendWM_SETFOCUS = true;
	bool focusLoopSendWM_MOUSEACTIVATE = true;
	int focuslooptimer = 5;

	std::vector<std::string> renameHandles{};
	std::vector<std::string> renameNamedPipeHandles{};

	std::vector<unsigned int> blockedMessages;
	
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("hooks", hooks),
			cereal::make_nvp("messageFilters", messageFilters),

			cereal::make_nvp("renameHandles", renameHandles),
			cereal::make_nvp("renameNamedPipeHandles", renameNamedPipeHandles),

			cereal::make_nvp("Reregisterinput", Reregisterinput),
			cereal::make_nvp("PointerInMouse", PointerInMouse),

			cereal::make_nvp("dinputToXinputRedirection", dinputToXinputRedirection),
			cereal::make_nvp("useOpenXinput", useOpenXinput),
			cereal::make_nvp("TranslateMKBtoXinput", TranslateMKBtoXinput),
			cereal::make_nvp("TranslateXinputtoMKB", TranslateXinputtoMKB),
			cereal::make_nvp("XinputtoMKBstickinvert", XinputtoMKBstickinvert),
			cereal::make_nvp("ScanOption", ScanOption),

			cereal::make_nvp("Shoulderswappoints", Shoulderswappoints),
			cereal::make_nvp("XAstatic", XAstatic),
			cereal::make_nvp("XAclick", XAclick),
			cereal::make_nvp("XAmove", XAmove),

			cereal::make_nvp("XBstatic", XBstatic),
			cereal::make_nvp("XBclick", XBclick),
			cereal::make_nvp("XBmove", XBmove),
			cereal::make_nvp("XXstatic", XXstatic),

			cereal::make_nvp("XXclick", XXclick),
			cereal::make_nvp("XXmove", XXmove),
			cereal::make_nvp("XYclick", XYclick),
			cereal::make_nvp("XYmove", XYmove),

			cereal::make_nvp("XinputtoMKBAkey", XinputtoMKBAkey),
			cereal::make_nvp("XinputtoMKBBkey", XinputtoMKBBkey),
			cereal::make_nvp("XinputtoMKBXkey", XinputtoMKBXkey),
			cereal::make_nvp("XinputtoMKBYkey", XinputtoMKBYkey),
			cereal::make_nvp("XinputtoMKBRSkey", XinputtoMKBRSkey),
			cereal::make_nvp("XinputtoMKBLSkey", XinputtoMKBLSkey),
			cereal::make_nvp("XinputtoMKBrightkey", XinputtoMKBrightkey),
			cereal::make_nvp("XinputtoMKBleftkey", XinputtoMKBleftkey),
			cereal::make_nvp("XinputtoMKBupkey", XinputtoMKBupkey),
			cereal::make_nvp("XinputtoMKBdownkey", XinputtoMKBdownkey),
			cereal::make_nvp("XinputtoMKBstickR", XinputtoMKBstickR),
			cereal::make_nvp("XinputtoMKBstickL", XinputtoMKBstickL),
			cereal::make_nvp("XinputtoMKBstickright", XinputtoMKBstickright),
			cereal::make_nvp("XinputtoMKBstickleft", XinputtoMKBstickleft),

			cereal::make_nvp("XinputtoMKBstickleft", XinputtoMKBstickleft),
			cereal::make_nvp("XinputtoMKBstickup", XinputtoMKBstickup),
			cereal::make_nvp("XinputtoMKBstickdown", XinputtoMKBstickdown),
			cereal::make_nvp("XinputtoMKBoption", XinputtoMKBoption),
			cereal::make_nvp("XinputtoMKBstart", XinputtoMKBstart),
			cereal::make_nvp("XinputtoMKBsens", XinputtoMKBsens),
			cereal::make_nvp("XinputtoMKBsensmult", XinputtoMKBsensmult),
			cereal::make_nvp("XinputtoMKBDeadzone", XinputtoMKBDeadzone),

			cereal::make_nvp("useFakeClipCursor", useFakeClipCursor),
			cereal::make_nvp("showCursorWhenImageUpdated", showCursorWhenImageUpdated),

			cereal::make_nvp("drawFakeMouseCursor", drawFakeMouseCursor),
			cereal::make_nvp("drawFakeCursorFix", drawFakeCursorFix),
			cereal::make_nvp("allowMouseOutOfBounds", allowMouseOutOfBounds),
			cereal::make_nvp("extendMouseBounds", extendMouseBounds),
			cereal::make_nvp("toggleFakeCursorVisibilityShortcut", toggleFakeCursorVisibilityShortcut),
			cereal::make_nvp("sendMouseMovementMessages", sendMouseMovementMessages),
			cereal::make_nvp("sendMouseButtonMessages", sendMouseButtonMessages),
			cereal::make_nvp("sendMouseWheelMessages", sendMouseWheelMessages),
			cereal::make_nvp("sendKeyboardButtonMessages", sendKeyboardButtonMessages),
			cereal::make_nvp("sendMouseDblClkMessages", sendMouseDblClkMessages),

			cereal::make_nvp("focusMessageLoop", focusMessageLoop),
			cereal::make_nvp("focusLoopSendWM_ACTIVATE", focusLoopSendWM_ACTIVATE),
			cereal::make_nvp("focusLoopSendWM_NCACTIVATE", focusLoopSendWM_NCACTIVATE),
			cereal::make_nvp("focusLoopSendWM_ACTIVATEAPP", focusLoopSendWM_ACTIVATEAPP),
			cereal::make_nvp("focusLoopSendWM_SETFOCUS", focusLoopSendWM_SETFOCUS),
			cereal::make_nvp("focusLoopSendWM_MOUSEACTIVATE", focusLoopSendWM_MOUSEACTIVATE),


			cereal::make_nvp("blockedMessages", blockedMessages)
		);
	}

	static void SaveToFile(const Profile& data, const std::string& name);
	static void LoadFromFile(Profile& data, const std::string& name);
	static bool DoesProfileFileAlreadyExist(const std::string& name);
	static std::vector<std::string> GetAllProfiles();
};

}