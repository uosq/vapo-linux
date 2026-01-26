#pragma once

#include "../vtables.h"
#include "../sdk/helpers/helper.h"
#include "../features/entitylist/entitylist.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"
#include "../features/lua/classes.h"

DECLARE_VTABLE_HOOK(LevelInitPreEntity, void, (IBaseClientDLL* thisptr, const char* mapName))
{
	if (LuaHookManager::HasHooks("LevelInitPreEntity"))
	{
		lua_pushstring(Lua::m_luaState, mapName);
		LuaHookManager::Call(Lua::m_luaState, "LevelInitPreEntity", 1);
	}

	originalLevelInitPreEntity(thisptr, mapName);
}

static void HookLevelInitPreEntity()
{
	INSTALL_VTABLE_HOOK(LevelInitPreEntity, interfaces::BaseClientDLL, 5);

	constexpr Color_t color{100, 255, 100, 255};
	helper::console::ColoredPrint("BaseClientDll::LevelInitPreEntity hooked\n", color);
}