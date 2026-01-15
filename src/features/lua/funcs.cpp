#include "funcs.h"
#include "../../gui/console.h"
#include <lua5.4/lua.h>
#include "hooks.h"

namespace LuaFuncs
{
	namespace common
	{
		void luaopen_commonfunctions(lua_State* L)
		{
			lua_register(L, "print", &Print);
		}

		int Print(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs == 0)
				return 0;

			for (int i = 1; i <= nargs; i++)
			{
				const char* str = luaL_tolstring(L, i, nullptr);
				if (str)
				{
					consoleText += std::string(str) + "\n";
				}
			}

			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace globalvars
	{
		void luaopen_globalvars(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, globalvarslib, 0);
			lua_setglobal(L, "globals");

			consoleText += "globals library initialized\n";
		}

		int TickCount(lua_State* L)
		{
			lua_pushinteger(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->tickcount : 0);
			return 1;
		}

		int TickInterval(lua_State* L)
		{
			lua_pushnumber(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->interval_per_tick : 0);
			return 1;
		}

		int CurTime(lua_State* L)
		{
			lua_pushnumber(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->curtime : 0);
			return 1;
		}

		int AbsoluteFrameTime(lua_State* L)
		{
			lua_pushnumber(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->absolute_frametime : 0);
			return 1;
		}

		int FrameTime(lua_State* L)
		{
			lua_pushnumber(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->frametime : 0);
			return 1;
		}

		int RealTime(lua_State* L)
		{
			lua_pushnumber(L, interfaces::GlobalVars != nullptr ? interfaces::GlobalVars->realtime : 0);
			return 1;
		}
	}
}

namespace LuaFuncs
{
	namespace hooks
	{
		int Register(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			const char* id = luaL_checkstring(L, 2);

			luaL_checktype(L, 3, LUA_TFUNCTION);
			lua_pushvalue(L, 3);
			int ref = luaL_ref(L, LUA_REGISTRYINDEX);

			LuaHookManager::Add(name, id, ref);

			//consoleText += "Hook " + std::string(id) + " registered\n";
			return 0;
		}

		int Unregister(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			const char* id = luaL_checkstring(L, 2);

			LuaHookManager::Remove(L, name, id);

			//consoleText += "Hook " + std::string(id) + " unregistered\n";
			return 0;
		}

		void luaopen_hooks(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, hooklib, 0);
			lua_setglobal(L, "hooks");

			//consoleText += "hooks library initialized\n";
		}
	}
}

namespace LuaFuncs
{
	namespace engine
	{
		void luaopen_engine(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, enginelib, 0);
			lua_setglobal(L, "engine");

			consoleText += "engine library initialized\n";
		}

		int IsInGame(lua_State* L)
		{
			lua_pushboolean(L, interfaces::Engine->IsInGame());
			return 1;
		}

		int IsConnected(lua_State* L)
		{
			lua_pushboolean(L, interfaces::Engine->IsConnected());
			return 1;
		}

		int IsTakingScreenshot(lua_State* L)
		{
			lua_pushboolean(L, interfaces::Engine->IsTakingScreenshot());
			return 1;
		}

		int IsGameUIVisible(lua_State* L)
		{
			lua_pushboolean(L, interfaces::EngineVGui->IsGameUIVisible());
			return 1;
		}
		
		int IsConsoleVisible(lua_State* L)
		{
			lua_pushboolean(L, interfaces::Engine->Con_IsVisible());
			return 1;
		}
	}
}