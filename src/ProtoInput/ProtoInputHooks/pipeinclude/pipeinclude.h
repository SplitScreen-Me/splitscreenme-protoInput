#pragma once

// This file needs to be used in both ProtoInputHooks and ProtoInputLoader for pipe communication

namespace ProtoPipe
{

enum class PipeMessageType
{
	AddSelectedMouseOrKeyboard,
	SetTranslateXinputtoMKB,
	SetupHook,
	WakeUpProcess,
	SetupMessageFilter,
	SetupMessageBlock,
	UpdateMainWindowHandle,
	StartFocusMessageLoop,
	SetupState,
	SetupMessagesToSend,
	SetDrawFakeCursor,
	SetDrawFakeCursorFix,
	SetExternalFreezeFakeInput,
	AddHandleToRename,
	SetControllerIndex,
	SetUseDinput,
	StopFocusMessageLoop,
	SetUseOpenXinput,
	SetDinputDeviceGuid,
	SetDinputHookGetDeviceState,
	SetSetWindowPosSettings,
	SetSetWindowPosDontResize,
	SetSetWindowPosDontReposition,
	SetCreateSingleHIDName,
	SetClipCursorHookOptions,
	SetAllowFakeCursorOutOfBounds,
	SetToggleCursorVisibilityShortcut,
	SetRawInputBypass,
	SetShowCursorWhenImageUpdated,
	SetPutMouseInsideWindow,
	SetDefaultTopLeftMouseBounds,
	SetDefaultBottomRightMouseBounds,
	SetMoveWindowSettings,
	SetMoveWindowDontResize,
	SetMoveWindowDontReposition,
	SetAdjustWindowRectSettings,
	SetDontWaitWindowBorder,
	SetManualScaling,
	SetXinputtoMKBkeys,
	SetXinputtoMKBCFG
};

struct PipeMessageHeader
{
	PipeMessageType messageType;
	unsigned int messageSize;
};
struct PipeMessageAddSelectedMouseOrKeyboard
{
	unsigned int mouse = -1;
	unsigned int keyboard = -1;
};

struct PipeMessageSetTranslateXinputtoMKB
{
	bool TranslateXinputtoMKB;
};


struct PipeMessageSetupHook
{
	ProtoHookIDs hookID;
	bool install;
};

struct PipeMessageSetupMessageFilter
{
	ProtoMessageFilterIDs filterID;
	bool enable;
};

struct PipeMessageSetupMessageBlock
{
	unsigned int message;
	bool block;
};

struct PipeMessageWakeUpProcess
{
};

//struct PipeMessageSetNoGUI
//{
//	bool SetNoGUI;
//};
struct PipeMesasgeUpdateMainWindowHandle
{
	uint64_t hwnd = 0;
};

struct PipeMessageStartFocusMessageLoop
{
	int milliseconds;
	bool wm_activate;
	bool wm_activateapp;
	bool wm_ncactivate;
	bool wm_setfocus;
	bool wm_mouseactivate;
};

struct PipeMessageSetupState
{
	int instanceNumber;
};

struct PipeMessageSetupMessagesToSend
{
	bool sendMouseWheelMessages;
	bool sendMouseButtonMessages;
	bool sendMouseMoveMessages;
	bool sendKeyboardPressMessages;
	bool sendMouseDblClkMessages;
};

struct PipeMessageSetDrawFakeCursor
{
	bool enable;
};
struct PipeMessageSetDrawFakeCursorFix
{
	bool enable;
};
struct PipeMessageSetExternalFreezeFakeInput
{
	bool freezeEnabled;
};



struct PipeMessageAddHandleToRename
{
	wchar_t buff[1000]{};
	bool isNamedPipe = false;
};

struct PipeMessageSetControllerIndex
{
	unsigned int controllerIndex;
	unsigned int controllerIndex2;
	unsigned int controllerIndex3;
	unsigned int controllerIndex4;
};

struct PipeMessageUseDinput
{
	bool useDinput;
};

struct PipeMessageStopFocusMessageLoop
{

};

struct PipeMessageSetUseOpenXinput
{
	bool useOpenXinput;
};

struct PipeMessageSetDinputDeviceGuid
{
	GUID guid;
};

struct PipeMessageSetDinputHookGetDeviceState
{
	bool enable;
};

struct PipeMessageSetSetWindowPosSettings
{
	int posx;
	int posy;
	int width;
	int height;
};

struct PipeMessageSetSetWindowPosDontResize
{
	bool SetWindowPosDontResize;
};

struct PipeMessageSetSetWindowPosDontReposition
{
	bool SetWindowPosDontReposition;
};

struct PipeMessageSetCreateSingleHIDName
{
	wchar_t buff[1000]{};
};

struct PipeMessageSetClipCursorHookOptions
{
	bool useFakeClipCursor;
};

struct PipeMessageSetAllowFakeCursorOutOfBounds
{
	bool allowOutOfBounds;
	bool extendBounds;
};

struct PipeMessageSetToggleCursorVisibilityShortcut
{
	bool enabled;
	unsigned int vkey;
};

struct PipeMessageSetRawInputBypass
{
	bool bypassEnabled;
};

struct PipeMessageShowCursorWhenImageUpdated
{
	bool ShowCursorWhenImageUpdated;
};

struct PipeMessagePutMouseInsideWindow
{
	bool PutMouseInsideWindow;
};

struct PipeMessageDefaultTopLeftMouseBounds
{
	bool DefaultTopLeftMouseBounds;
};

struct PipeMessageDefaultBottomRightMouseBounds
{
	bool DefaultBottomRightMouseBounds;
};

struct PipeMessageSetMoveWindowSettings
{
	int posx;
	int posy;
	int width;
	int height;
};

struct PipeMessageSetMoveWindowDontResize
{
	bool MoveWindowDontResize;
};

struct PipeMessageSetMoveWindowDontReposition
{
	bool MoveWindowDontReposition;
};

struct PipeMessageSetAdjustWindowRectSettings
{
	int posx;
	int posy;
	int width;
	int height;
};

struct PipeMessageSetDontWaitWindowBorder
{
	bool DontWaitWindowBorder;
};

struct PipeMessageSetManualScaling
{
	int oldX;
	int oldY;
	int newX;
	int newY;
};

struct PipeMessageSetXinputtoMKBkeys
{
	int XinputtoMKBAkey;
	int XinputtoMKBBkey;
	int XinputtoMKBXkey;
	int XinputtoMKBYkey;
	int XinputtoMKBRSkey;
	int XinputtoMKBLSkey;
	int XinputtoMKBrightkey;
	int XinputtoMKBleftkey;
	int XinputtoMKBupkey;
	int XinputtoMKBdownkey;
	int XinputtoMKBstickR;
	int XinputtoMKBstickL;
	int XinputtoMKBstickright;
	int XinputtoMKBstickleft;
	int XinputtoMKBstickup;
	int XinputtoMKBstickdown;
	int XinputtoMKBoption;
	int XinputtoMKBstart;
	int XinputtoMKBsens;
	int XinputtoMKBsensmult;
};
struct PipeMessageSetXinputtoMKBCFG
{
	bool stickinvert;
	bool scanoption;
	bool shoulderswap;
	bool astsatic;
	bool aclick;
	bool amove;
	bool bstsatic;
	bool bclick;
	bool bmove;
	bool xstsatic;
	bool xclick;
	bool xmove;
	bool ystsatic;
	bool yclick;
	bool ymove;
};
}