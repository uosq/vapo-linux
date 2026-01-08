#pragma once

#include "../../sdk/definitions/cusercmd.h"
#include "../../sdk/classes/player.h"
#include "../../settings.h"

namespace Bhop
{
	inline void Run(CTFPlayer* pLocal, CUserCmd* pCmd)
	{
		if (!settings.misc.bhop || pLocal->m_nWaterLevel() >= WL_Feet)
			return;

		static bool usedAirJump = false;
		static bool jumpReleased = false;

		bool wantsJump = pCmd->buttons & IN_JUMP;
		bool isOnGround = pLocal->GetFlags() & FL_ONGROUND;
		bool isScout = pLocal->m_iClass() == TF_CLASS_SCOUT;

		if (isOnGround)
		{
			usedAirJump = false;
			jumpReleased = false;

			if (wantsJump)
				pCmd->buttons |= IN_JUMP;
		}
		else
		{
			if (!wantsJump)
				jumpReleased = true;

			if (isScout && wantsJump && !usedAirJump && jumpReleased)
			{
				pCmd->buttons |= IN_JUMP;
				usedAirJump = true;
			}
			else
			{
				pCmd->buttons &= ~IN_JUMP;
			}
		}
	}
}