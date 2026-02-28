#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class Scaler
	{
	private:
		static void Install();
		static void Uninstall();

	public:
		static POINT getfactor(POINT pp);
		
		static void Settings(int oldX, int oldY, int newX, int newY);
	};

}
