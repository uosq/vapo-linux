#pragma once

#include "../sdk/definitions/igameevents.h"
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../settings.h"
#include "../libdetour/libdetour.h"
#include "../sdk/definitions/inetchannel.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"
#include "../features/lua/classes.h"

// Ideally instead of a hook i should just get ClientModeShared and use it instead

using FireGameEventFn = void(*)(void* self, IGameEvent* gameEvent);
inline FireGameEventFn original_FireGameEvent = nullptr;
inline detour_ctx_t firegameevent_ctx;

DETOUR_DECL_TYPE(void, original_FireGameEvent, void* self, IGameEvent* gameEvent);

inline void Hooked_FireGameEvent(void* self, IGameEvent* event)
{
	if (event == nullptr)
	{
		DETOUR_ORIG_CALL(&firegameevent_ctx, original_FireGameEvent, self, event);
		return;
	}

	if (LuaHookManager::HasHooks("FireGameEvent"))
	{
		LuaClasses::GameEventLua::push_gameevent(Lua::m_luaState, event);
		LuaHookManager::Call(Lua::m_luaState, "FireGameEvent", 1);
	}

	DETOUR_ORIG_CALL(&firegameevent_ctx, original_FireGameEvent, self, event);
}

inline void HookFireGameEvent(void)
{
	// xref: #game_player_joined_game
	original_FireGameEvent = reinterpret_cast<FireGameEventFn>(sigscan_module("client.so", "55 48 89 E5 41 57 41 56 49 89 FE 41 55 41 54 49 89 F4 53 48 8D 35"));
	detour_init(&firegameevent_ctx, (void*)original_FireGameEvent, (void*)&Hooked_FireGameEvent);
	detour_enable(&firegameevent_ctx);

	interfaces::Cvar->ConsolePrintf("ClientModeShared::FireGameEvent hooked\n");
}