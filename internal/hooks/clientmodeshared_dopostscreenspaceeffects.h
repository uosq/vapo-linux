#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"

#include "../features/chams/chams.h"

DECLARE_VTABLE_HOOK(DoPostScreenSpaceEffects, bool, (IClientMode* thisptr, CViewSetup* setup))
{
	Chams::Run();

	//helper::console::Print("DoPost\n");

	return originalDoPostScreenSpaceEffects(thisptr, setup);
}

inline void HookDoPostScreenSpaceEffects(void)
{
	INSTALL_VTABLE_HOOK(DoPostScreenSpaceEffects, interfaces::ClientMode, 40);
	helper::console::ColoredPrint("Hooked ClientMode::DoPostScreenSpaceEffects\n", (Color_t){100, 255, 100, 255});
}