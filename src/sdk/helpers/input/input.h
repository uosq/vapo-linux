#pragma once

#include "../../interfaces/interfaces.h"
#include <string>

namespace helper
{
	namespace input
	{
		static bool IsButtonValid(ButtonCode_t btn)
		{
			return btn != BUTTON_CODE_INVALID;
		}

		static bool CheckButtonPressedTick(ButtonCode_t btn)
		{
			static int lasttickpressed = 0;
			int tick = interfaces::InputSystem->GetButtonPressedTick(btn);
			if (tick > lasttickpressed)
			{
				lasttickpressed = tick;
				return true;
			}

			return false;
		}

		// I still haven't written this function
		// But I bet the code will look horrible
		static bool IsKeyPressed(std::string key)
		{
			ButtonCode_t btn = interfaces::InputSystem->StringToButtonCode(key.c_str());

			if (IsButtonValid(btn) && CheckButtonPressedTick(btn))
				return true;

			btn = interfaces::InputSystem->StringToButtonCode(("kp_" + key).c_str());
			if (IsButtonValid(btn) && CheckButtonPressedTick(btn))
				return true;

			return false;
		}
	}
}