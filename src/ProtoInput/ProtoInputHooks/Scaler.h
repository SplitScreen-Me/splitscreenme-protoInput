#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{

	class Scaler
	{
	public:
		static POINT getfactor(POINT pp);
		static void Install();
		static void Settings(bool enabled, int scaletoX, int scaletoY);
	};

}
