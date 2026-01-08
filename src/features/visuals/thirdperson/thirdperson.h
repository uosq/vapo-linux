#pragma once

#include "../../../sdk/interfaces/interfaces.h"
#include "../../../sdk/helpers/helper.h"
#include "../../../settings.h"

struct Thirdperson
{
	void RunHotkey()
	{
		if (settings.misc.thirdperson_key == "")
			return;

		if (helper::input::IsKeyPressed(settings.misc.thirdperson_key))
			settings.misc.thirdperson = !settings.misc.thirdperson;
	}

	// Call in FrameStageNotify -> FRAME_NET_UPDATE_END
	void Run(CTFPlayer* pLocal)
	{
		RunHotkey();

		if (!pLocal || !pLocal->IsAlive())
			return;

		if (settings.misc.thirdperson)
			pLocal->m_nForceTauntCam() = 1;

		if (!settings.misc.thirdperson && interfaces::CInput->CAM_IsThirdPerson())
			pLocal->m_nForceTauntCam() = 0;
	}
};