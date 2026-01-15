#pragma once

#include "../../sdk/interfaces/interfaces.h"

extern "C"
{
	#include <lua5.4/lua.h>
	#include <lua5.4/lua.hpp>
	#include <lua5.4/lualib.h>
}

namespace LuaFuncs
{
	namespace common
	{
		void luaopen_commonfunctions(lua_State* L);
		int Print(lua_State* L);
	}

	namespace globalvars
	{
		void luaopen_globalvars(lua_State* L);
		int TickCount(lua_State* L);
		int TickInterval(lua_State* L);
		int CurTime(lua_State* L);
		int AbsoluteFrameTime(lua_State* L);
		int FrameTime(lua_State* L);
		int RealTime(lua_State* L);

		const luaL_Reg globalvarslib[] =
		{
			{"TickCount", &TickCount},
			{"TickInterval", &TickInterval},
			{"CurTime", &CurTime},
			{"AbsoluteFrameTime", &AbsoluteFrameTime},
			{"FrameTime", &FrameTime},
			{"RealTime", &RealTime},
			{nullptr, nullptr},
		};
	}

	namespace engine
	{
		void luaopen_engine(lua_State* L);
		int IsInGame(lua_State* L);
		int IsConnected(lua_State* L);
		int IsTakingScreenshot(lua_State* L);
		int IsGameUIVisible(lua_State* L);
		int IsConsoleVisible(lua_State* L);

		const luaL_Reg enginelib[] =
		{
			{"IsInGame", IsInGame},
			{"IsConnected", IsConnected},
			{"IsTakingScreenshot", IsTakingScreenshot},
			{"IsGameUIVisible", IsGameUIVisible},
			{"IsConsoleVisible", IsConsoleVisible},
			{nullptr, nullptr},
		};
	}

	namespace hooks
	{
		void luaopen_hooks(lua_State* L);
		int Register(lua_State* L);
		int Unregister(lua_State* L);

		const luaL_Reg hooklib[] =
		{
			{"Add", Register},
			{"Remove", Unregister},
			{nullptr, nullptr},
		};
	}
}