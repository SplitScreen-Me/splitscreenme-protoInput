#include "HookManager.h"
#include "RegisterRawInputHook.h"
#include "GetRawInputDataHook.h"
#include "MessageFilterHook.h"
#include "FocusHook.h"
#include "GetCursorPosHook.h"
#include "SetCursorPosHook.h"
#include "GetKeyStateHook.h"
#include "GetAsyncKeyStateHook.h"
#include "GetKeyboardStateHook.h"
#include "CursorVisibilityHook.h"
#include "ClipCursorHook.h"
#include "RenameHandlesHook.h"
#include "XinputHook.h"
#include "DinputOrderHook.h"
#include "SetWindowPosHook.h"
#include "BlockRawInputHook.h"
#include "FindWindowHook.h"
#include "CreateSingleHIDHook.h"
#include "WindowStyleHook.h"
#include "MoveWindowHook.h"
#include "AdjustWindowRectHook.h"
#include "RemoveBorderHook.h"
#include "GetCursorInfoHook.h"
#include "SetWindowsHookHook.h"
#include "RawInput.h"
#include "TranslateXtoMKB.h"

namespace Proto
{

HookManager HookManager::hookManagerInstance{};

HookManager::HookManager()
{
	// Do these in exactly the same order as in ProtoHookIDs
	AddHook<RegisterRawInputHook>(ProtoHookIDs::RegisterRawInputHookID); //0
	AddHook<GetRawInputDataHook>(ProtoHookIDs::GetRawInputDataHookID); //1
	AddHook<MessageFilterHook>(ProtoHookIDs::MessageFilterHookID);//2
	AddHook<GetCursorPosHook>(ProtoHookIDs::GetCursorPosHookID);//3
	AddHook<SetCursorPosHook>(ProtoHookIDs::SetCursorPosHookID);//4
	AddHook<GetKeyStateHook>(ProtoHookIDs::GetKeyStateHookID);//5
	AddHook<GetAsyncKeyStateHook>(ProtoHookIDs::GetAsyncKeyStateHookID);//6
	AddHook<GetKeyboardStateHook>(ProtoHookIDs::GetKeyboardStateHookID);//7
	AddHook<CursorVisibilityHook>(ProtoHookIDs::CursorVisibilityStateHookID);//8
	AddHook<ClipCursorHook>(ProtoHookIDs::ClipCursorHookID);//9
	AddHook<FocusHook>(ProtoHookIDs::FocusHooksHookID);//10
	AddHook<RenameHandlesHook>(ProtoHookIDs::RenameHandlesHookID);//11
	AddHook<XinputHook>(ProtoHookIDs::XinputHookID);//12
	AddHook<DinputOrderHook>(ProtoHookIDs::DinputOrderHookID);//13
	AddHook<SetWindowPosHook>(ProtoHookIDs::SetWindowPosHookID);//14
	AddHook<BlockRawInputHook>(ProtoHookIDs::BlockRawInputHookID);//15
	AddHook<FindWindowHook>(ProtoHookIDs::FindWindowHookID);//16
	AddHook<CreateSingleHIDHook>(ProtoHookIDs::CreateSingleHIDHookID);//17
	AddHook<WindowStyleHook>(ProtoHookIDs::WindowStyleHookID);//18
	AddHook<MoveWindowHook>(ProtoHookIDs::MoveWindowHookID);//19
	AddHook<AdjustWindowRectHook>(ProtoHookIDs::AdjustWindowRectHookID);//20
	AddHook<RemoveBorderHook>(ProtoHookIDs::RemoveBorderHookID);//21
	AddHook<GetCursorInfoHook>(ProtoHookIDs::GetCursorInfoHookID);//22
	AddHook<SetWindowsHookHook>(ProtoHookIDs::SetWindowsHookHookID);//22
}

void HookManager::InstallHook(ProtoHookIDs hookID)
{

	if (hookID < 0 || hookID >= hookManagerInstance.hooks.size())
		std::cerr << "Trying to install hook ID " << hookID << " which is out of range" << std::endl;
	else
	{
		hookManagerInstance.hooks[hookID]->Install();
	}
}

void HookManager::UninstallHook(ProtoHookIDs hookID)
{
	if (hookID < 0 || hookID >= hookManagerInstance.hooks.size())
		std::cerr << "Trying to uninstall hook ID " << hookID << " which is out of range" << std::endl;
	else
	{
		hookManagerInstance.hooks[hookID]->Uninstall();
	}
}

bool HookManager::IsInstalled(ProtoHookIDs hookID)
{
	if (hookID < 0 || hookID >= hookManagerInstance.hooks.size())
	{
		std::cerr << "Trying to check hook ID " << hookID << " which is out of range" << std::endl;
		return false;
	}
	else
	{
		return hookManagerInstance.hooks[hookID]->IsInstalled();
	}
}

}
