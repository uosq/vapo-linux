#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/helpers/helper.h"
#include "../features/visuals/visuals.h"

DECLARE_VTABLE_HOOK(FrameStageNotify, void, (IBaseClientDLL* thisptr, int stage))
{
	switch (stage)
	{
		case FRAME_RENDER_START:
		{
			if (settings.misc.thirdperson)
			{
				CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
				if (!pLocal || !pLocal->IsAlive())
					break;

				interfaces::Prediction->SetLocalViewAngles(helper::localplayer::LastAngle);
			}
			break;
		}
		case FRAME_NET_UPDATE_END:
		{
			CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
			if (!pLocal)
				break;

			Visuals::thirdperson.Run(pLocal);
			break;
		}

		default: break;
	}

	originalFrameStageNotify(thisptr, stage);
}

inline void HookFrameStageNotify()
{
	INSTALL_VTABLE_HOOK(FrameStageNotify, interfaces::BaseClientDLL, 35);

	Color_t color;
	color.SetRGB(100, 255, 100, 255);
	helper::console::ColoredPrint("IBaseClientDll::FrameStageNotify hooked\n", color);
}