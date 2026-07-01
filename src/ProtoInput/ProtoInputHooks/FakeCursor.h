#pragma once
#include <mutex>
#include <windows.h>

namespace Proto
{

	class FakeCursor
	{
		std::mutex mutex{};
		std::condition_variable conditionvar{};

		HWND pointerWindow = nullptr;
		HDC hdc;
		HBRUSH transparencyBrush;
		HCURSOR hCursor;

		static constexpr auto transparencyKey = RGB(0, 0, 1);

		int oldX, oldY;
		bool oldHadShowCursor = true;
		//TODO: width/height probably needs to change


		// DrawFakeCursorFix. cursor offset scan and cursor size fix
		int cursoroffsetx, cursoroffsety;
		int offsetSET = 0; //0:sizing 1:scanning 2:done, only drawing until cursor change, or nochange
		int cursorWidth = 40; //was constant
		int cursorHeight = 40;//was constant
		bool nochange = false; //if normal offset was found at first then assume all cursors got same offset
		HCURSOR oldhCursor = NULL;
		POINT OldTestpos = { 0,0 };

		// This is either on or off for a given game (ie. it doesn't change)
		bool drawingEnabled = false;

		// This changes when the hook detects SetCursor/ShowCursor
		bool showCursor = true;

		bool toggleVisilbityShorcutEnabled = false;
		unsigned int toggleVisibilityVkey = VK_HOME;

		

		//TranslateXtoMKB
		POINT OldspotA, OldspotB, OldspotX, OldspotY;
		int oldmessage = 0;
		bool messageshown = false;
		HWND selectorhwnd = nullptr; //copy of variable in TranslateXtoMKB to avoid accessing it multiple times with critical section in DrawCursor

		void DrawMessage(HDC hdc, HWND window, HBRUSH Brush, int message);
		void DrawFoundSpots(HDC hdc, POINT spotA, POINT spotB, POINT spotX, POINT spotY, HWND window, HBRUSH Brush);
		void DrawPointsandMessages();
		void DrawCursor();


public:
	static FakeCursor state;
	static bool DrawFakeCursorFix;
	void StartInternal();
	void StartDrawLoopInternal();

	static int Showmessage;
	static bool& GetToggleVisilbityShorcutEnabled()
	{
		return state.toggleVisilbityShorcutEnabled;
	}

	static unsigned int& GetToggleVisibilityVkey()
	{
		return state.toggleVisibilityVkey;
	}
	
	static void NotifyUpdatedCursorPosition()
	{
		state.conditionvar.notify_one();
	}

	static bool IsDrawingEnabled()
	{
		return state.drawingEnabled;
	}

	static HWND GetPointerWindow()
	{
		return state.pointerWindow;
	}

	static void SetCursorVisibility(bool visible)
	{
		state.showCursor = visible;
		NotifyUpdatedCursorPosition();
	}

	static bool GetCursorVisibility()
	{
		return state.showCursor;
	}

	static void SetCursorHandle(HCURSOR newCursor)
	{
		state.hCursor = newCursor;
	}
	
	static void EnableDisableFakeCursor(bool enable);

	static void Initialise(HMODULE module);
};

}
