#pragma once

#include "../../../sdk/interfaces/interfaces.h"
#include "../../../sdk/helpers/helper.h"
#include "../../../settings.h"

struct Thirdperson
{
	// Call in FrameStageNotify -> FRAME_NET_UPDATE_END
	void Run(CTFPlayer* pLocal)
	{
		if (!pLocal || !pLocal->IsAlive())
			return;

		if (settings.misc.thirdperson)
			pLocal->m_nForceTauntCam() = 1;

		if (!settings.misc.thirdperson && pLocal->m_nForceTauntCam() && pLocal->IsAlive() && !pLocal->IsGhost())
			pLocal->m_nForceTauntCam() = 0;
	}
};