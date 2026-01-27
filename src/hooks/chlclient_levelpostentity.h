#pragma once

#include "../vtables.h"
#include "../sdk/helpers/helper.h"
#include "../features/entitylist/entitylist.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"
#include "../features/lua/classes.h"

DECLARE_VTABLE_HOOK(LevelInitPostEntity, void, (CHLClient* thisptr))
{
	if (LuaHookManager::HasHooks("LevelInitPostEntity"))
		LuaHookManager::Call(Lua::m_luaState, "LevelInitPostEntity", 0);

	originalLevelInitPostEntity(thisptr);
}

static void HookLevelInitPostEntity()
{
	INSTALL_VTABLE_HOOK(LevelInitPostEntity, interfaces::ClientDLL, 6);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("BaseClientDll::LevelInitPostEntity hooked\n", color);
}