#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"

#include "../features/chams/chams.h"
#include "../features/glow/glow.h"

DECLARE_VTABLE_HOOK(DoPostScreenSpaceEffects, bool, (IClientMode* thisptr, CViewSetup* setup))
{
	Chams::Run();
	Glow::Run();

	//helper::console::Print("DoPost\n");

	return originalDoPostScreenSpaceEffects(thisptr, setup);
}

inline void HookDoPostScreenSpaceEffects(void)
{
	INSTALL_VTABLE_HOOK(DoPostScreenSpaceEffects, interfaces::ClientMode, 40);
	helper::console::ColoredPrint("ClientMode::DoPostScreenSpaceEffects Hooked\n", (Color_t){100, 255, 100, 255});
}