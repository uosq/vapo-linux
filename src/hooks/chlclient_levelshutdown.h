#pragma once

#include "../vtables.h"
#include "../sdk/helpers/helper.h"
#include "../features/entitylist/entitylist.h"
#include "../features/lua/hooks.h"
#include "../features/lua/api.h"
#include "../features/lua/classes.h"

DECLARE_VTABLE_HOOK(LevelShutdown, void, (CHLClient* thisptr))
{
	EntityList::Clear();

	if (LuaHookManager::HasHooks("LevelShutdown"))
		LuaHookManager::Call(Lua::m_luaState, "LevelShutdown");

	originalLevelShutdown(thisptr);
}

static void HookLevelShutdown()
{
	INSTALL_VTABLE_HOOK(LevelShutdown, interfaces::ClientDLL, 7);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("BaseClientDll::LevelShutdown hooked\n", color);
}