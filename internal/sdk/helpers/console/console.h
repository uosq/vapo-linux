#pragma once

#include "../../interfaces/interfaces.h"

namespace helper
{
	namespace console
	{
		inline void Print(const char* text)
		{
			interfaces::Cvar->ConsolePrintf(text);
		}

		inline void ColoredPrint(const char* text, Color_t color)
		{
			interfaces::Cvar->ConsoleColorPrintf(color, text);
		}
	}
};