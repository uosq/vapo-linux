#pragma once

// sig 55 48 89 E5 41 56 41 89 D6 41 55 41 89 CD

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

using SendNetMsgFn = bool (*)(void* ptr, INetMessage& msg, bool bForceReliable, bool bVoice);
inline SendNetMsgFn originalSendNetMsg = nullptr;
inline detour_ctx_t SendNetMsg_ctx;

DETOUR_DECL_TYPE(bool, originalSendNetMsg, void* ptr, INetMessage& msg, bool bForceReliable, bool bVoice);

inline int BfWriteToBuffer(lua_State* L);

inline bool Hooked_SendNetMsg(void* ptr, INetMessage& msg, bool bForceReliable, bool bVoice)
{
	if (LuaHookManager::HasHooks("SendNetMsg"))
	{
		LuaClasses::NetMessageLua::push_netmessage(Lua::m_luaState, &msg);
		LuaHookManager::Call(Lua::m_luaState, "SendNetMsg", 1);
	}

	bool ret;
	DETOUR_ORIG_GET(&SendNetMsg_ctx, ret, originalSendNetMsg, ptr, msg, bForceReliable, bVoice);
	return ret;
}

inline void HookSendNetMsg()
{
	// xref: NetMsg
	originalSendNetMsg = reinterpret_cast<SendNetMsgFn>(sigscan_module("engine.so", "55 48 89 E5 41 56 41 89 D6 41 55 41 89 CD"));
	detour_init(&SendNetMsg_ctx, (void*)originalSendNetMsg, (void*)&Hooked_SendNetMsg);
	detour_enable(&SendNetMsg_ctx);

	Color_t color{100, 255, 100, 255};
	interfaces::Cvar->ConsoleColorPrintf(color, "CNetChan::SendNetMsg hooked\n");
}