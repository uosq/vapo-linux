#pragma once

#include <string>
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../features/aimbot/aimbot.h"
#include "../features/bhop/bhop.h"
#include "../features/triggerbot/triggerbot.h"
#include "../features/entitylist/entitylist.h"
#include "../features/antiaim/antiaim.h"
#include "../features/lua/hooks.h"
#include "../features/lua/api.h"

// Source https://8dcc.github.io/reversing/reversing-tf2-bsendpacket.html#introduction
#define SENDPACKET_STACK_OFFSET 0xF8

inline void RunLuaCreateMoveCallback(CUserCmd* pCmd, bool* bSendPacket)
{
	lua_newtable(Lua::m_luaState);

	lua_pushstring(Lua::m_luaState, "buttons");
	lua_pushinteger(Lua::m_luaState, pCmd->buttons);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "tick_count");
	lua_pushinteger(Lua::m_luaState, pCmd->tick_count);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "forwardmove");
	lua_pushnumber(Lua::m_luaState, pCmd->forwardmove);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "sidemove");
	lua_pushnumber(Lua::m_luaState, pCmd->sidemove);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "upmove");
	lua_pushnumber(Lua::m_luaState, pCmd->upmove);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "mousedx");
	lua_pushinteger(Lua::m_luaState, pCmd->mousedx);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "mousedy");
	lua_pushinteger(Lua::m_luaState, pCmd->mousedy);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "weaponselect");
	lua_pushinteger(Lua::m_luaState, pCmd->weaponselect);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "weaponsubtype");
	lua_pushinteger(Lua::m_luaState, pCmd->weaponsubtype);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "impulse");
	lua_pushinteger(Lua::m_luaState, pCmd->impulse);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "hasbeenpredicted");
	lua_pushboolean(Lua::m_luaState, pCmd->hasbeenpredicted);
	lua_settable(Lua::m_luaState, -3);

	lua_pushstring(Lua::m_luaState, "sendpacket");
	lua_pushboolean(Lua::m_luaState, *bSendPacket);
	lua_settable(Lua::m_luaState, -3);

	LuaHookManager::Call(Lua::m_luaState, "CreateMove", 1, false);

	lua_getfield(Lua::m_luaState, -1, "buttons");
	pCmd->buttons = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "tick_count");
	pCmd->tick_count = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "forwardmove");
	pCmd->forwardmove = lua_tonumber(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "sidemove");
	pCmd->sidemove = lua_tonumber(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "upmove");
	pCmd->upmove = lua_tonumber(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "mousedx");
	pCmd->mousedx = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "mousedy");
	pCmd->mousedy = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "weaponselect");
	pCmd->weaponselect = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "weaponsubtype");
	pCmd->weaponsubtype = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "impulse");
	pCmd->impulse = lua_tointeger(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "hasbeenpredicted");
	pCmd->hasbeenpredicted = lua_toboolean(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_getfield(Lua::m_luaState, -1, "sendpacket");
	*bSendPacket = lua_toboolean(Lua::m_luaState, -1);
	lua_pop(Lua::m_luaState, 1);

	lua_pop(Lua::m_luaState, 1); // pop the table
}

DECLARE_VTABLE_HOOK(CreateMove, bool, (IClientMode* thisptr, float sample_frametime, CUserCmd* pCmd))
{
	bool ret = originalCreateMove(thisptr, sample_frametime, pCmd);

	if (!pCmd || !pCmd->command_number)
		return ret;

	if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
		return ret;

	EntityList::m_pAimbotTarget = nullptr;

	CTFPlayer* pLocal = EntityList::GetLocal();
	if (!pLocal || !pLocal->IsAlive() || pLocal->IsTaunting())
		return ret;

	CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase*>(pLocal->GetActiveWeapon());
	if (pWeapon == nullptr)
		return ret;

	Vector originalAngles = pCmd->viewangles;

	uintptr_t current_frame_address = reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
    	uintptr_t current_stack_address = current_frame_address + 0x8;
    	bool* bSendPacket = (bool*)(current_stack_address + SENDPACKET_STACK_OFFSET);

	Bhop::Run(pLocal, pCmd);
	Antiaim::Run(pLocal, pWeapon, pCmd, bSendPacket);
	Aimbot::Run(pLocal, pWeapon, pCmd, bSendPacket);
	Triggerbot::Run(pLocal, pWeapon, pCmd);

	if (LuaHookManager::HasHooks("CreateMove"))
		RunLuaCreateMoveCallback(pCmd, bSendPacket);

	if (reinterpret_cast<CClientState*>(interfaces::ClientState)->chokedcommands >= 21)
		*bSendPacket = true;

	if (*bSendPacket == true)
		helper::localplayer::LastAngle = pCmd->viewangles;

	helper::engine::FixMovement(pCmd, originalAngles, pCmd->viewangles);
	// Return false so the engine doesn't apply it to engine->SetViewAngles; (this is stupid)
	return false;
}

inline void HookCreateMove()
{
	INSTALL_VTABLE_HOOK(CreateMove, interfaces::ClientMode, 22);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("ClientModeShared::CreateMove hooked\n", color);
}