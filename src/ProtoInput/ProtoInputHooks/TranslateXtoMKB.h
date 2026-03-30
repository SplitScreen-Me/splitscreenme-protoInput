#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace ScreenshotInput
{

	class TranslateXtoMKB 
	{
	private:
	//	static bool usetraqnslation;

	public:
		static void Initialize(HMODULE g_hModule);
		static void ThreadFunction(); //polling from idle drawfakecursor thread
		static void SendMouseClick(int x, int y, int z);
		static int RefreshWindow;
		static int RefreshPoint;
		static int controllerID;
		static bool rawinputhook;
		static bool registerrawinputhook;
		//static int showmessage;
		static int mode;
		static int Amapping;
		static int Bmapping;
		static int Xmapping;
		static int Ymapping;
		static int RSmapping;
		static int LSmapping;
		static int rightmapping;
		static int leftmapping;
		static int upmapping;
		static int downmapping;
		static int stickRpressmapping;
		static int stickLpressmapping;
		static int stickrightmapping;
		static int stickleftmapping;
		static int stickupmapping;
		static int stickdownmapping;
		static int optionmapping;
		static int startmapping;
		static bool lefthanded;
		static int Sens;
		static int Sensmult;
		static int Deadzone;
		static bool SaveBmps;
	};

}