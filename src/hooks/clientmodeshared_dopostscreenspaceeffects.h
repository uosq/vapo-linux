#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"

#include "../features/chams/chams.h"
#include "../features/glow/glow.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"

DECLARE_VTABLE_HOOK(DoPostScreenSpaceEffects, bool, (IClientMode* thisptr, CViewSetup* setup))
{
	if (LuaHookManager::HasHooks("DoPostScreenSpaceEffects"))
		LuaHookManager::Call(Lua::m_luaState, "DoPostScreenSpaceEffects", 0);

	Chams::Run();
	Glow::Run();

	return originalDoPostScreenSpaceEffects(thisptr, setup);
}

inline void HookDoPostScreenSpaceEffects(void)
{
	INSTALL_VTABLE_HOOK(DoPostScreenSpaceEffects, interfaces::ClientMode, 40);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("ClientMode::DoPostScreenSpaceEffects Hooked\n", color);
}