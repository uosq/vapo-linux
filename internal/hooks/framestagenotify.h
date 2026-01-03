#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/helpers/helper.h"
#include "../features/visuals/visuals.h"

using FrameStageNotifyFn = void (*)(IBaseClientDLL* thisptr, int stage);
inline FrameStageNotifyFn originalFrameStage = nullptr;

inline void HookedFrameStageNotify(IBaseClientDLL* thisptr, int stage)
{
	originalFrameStage(thisptr, stage);

	switch (stage)
	{
		case FRAME_NET_UPDATE_END:
		{
			CTFPlayer* pLocal = helper::engine::GetLocalPlayer();
			if (!pLocal)
				return;

			Visuals::thirdperson.Run(pLocal);
			return;
		}

		default: return;
	}
}

inline void HookFrameStageNotify()
{
	auto vt = vtable::get(interfaces::BaseClientDLL);
	originalFrameStage = vtable::hook(vt, 35, &HookedFrameStageNotify);
	
	/*
		reminder to me:
		if using windows hooks, always add a +1, as (i think)
		gcc/clang puts the constructor&destructor at the start of the class
	*/

	Color_t color;
	color.SetRGB(100, 255, 100, 255);
	helper::console::ColoredPrint("IBaseClientDll::FrameStageNotify hooked\n", color);
}