#pragma once

#include "../sdk/definitions/ipanel.h"
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include "cbaseviewmodel_calcviewmodelview.h"

DECLARE_VTABLE_HOOK(PaintTraverse, void, (IPanel* thisptr, VPANEL vguiPanel, bool forceRepaint, bool allowForce))
{
	if (!helper::engine::IsInMatch())
		stoptime = 0.0f;

	const char* panelName = interfaces::VGui->GetName(vguiPanel);

	// https://github.com/rei-2/Amalgam/blob/master/Amalgam/src/Hooks/IPanel_PaintTraverse.cpp
	if (settings.misc.streamer_mode)
	{
		switch (fnv::Hash(panelName))
		{
			case fnv::HashConst("SteamFriendsList"):
			case fnv::HashConst("avatar"):
			case fnv::HashConst("RankPanel"):
			case fnv::HashConst("ModelContainer"):
			case fnv::HashConst("ServerLabelNew"):
			return;
		}
	}

	originalPaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);
}

inline void HookPaintTraverse()
{
	INSTALL_VTABLE_HOOK(PaintTraverse, interfaces::VGui, 42);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("IPanel::PaintTraverse hooked\n", color);
}