#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../features/entitylist/entitylist.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include <string>

DECLARE_VTABLE_HOOK(OverrideView, void, (IClientMode *thisptr, CViewSetup *view))
{
	originalOverrideView(thisptr, view);

	if (view == nullptr)
		return;

	if (CTFPlayer* pLocal = EntityList::GetLocal(); pLocal != nullptr)
	{
		if (!pLocal->IsAlive() || pLocal->InCond(ETFCond::TF_COND_ZOOMED))
			return;

		if (settings.misc.customfov_enabled)
			view->fov = settings.misc.customfov;
	}
}

inline void HookOverrideView()
{
	INSTALL_VTABLE_HOOK(OverrideView, interfaces::ClientMode, 17);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("ClientModeShared::OverrideView hooked\n", color);
}