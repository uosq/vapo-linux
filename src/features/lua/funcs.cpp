#include "funcs.h"
#include "../../gui/console.h"
#include <lua5.4/lua.h>
#include "classes.h"
#include "hooks.h"
#include "../entitylist/entitylist.h"

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

		void luaopen_globalvars(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, globalvarslib, 0);
			lua_setglobal(L, "globals");

			//consoleText += "globals library initialized\n";
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
		const luaL_Reg hooklib[] =
		{
			{"Add", Register},
			{"Remove", Unregister},
			{nullptr, nullptr},
		};

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
		const luaL_Reg enginelib[] =
		{
			{"IsInGame", IsInGame},
			{"IsConnected", IsConnected},
			{"IsTakingScreenshot", IsTakingScreenshot},
			{"IsGameUIVisible", IsGameUIVisible},
			{"IsConsoleVisible", IsConsoleVisible},
			{"GetViewAngles", GetViewAngles},
			{"SetViewAngles", SetViewAngles},
			{nullptr, nullptr},
		};

		void luaopen_engine(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, enginelib, 0);
			lua_setglobal(L, "engine");

			//consoleText += "engine library initialized\n";
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

		int GetViewAngles(lua_State* L)
		{
			Vector angles;
			interfaces::Engine->GetViewAngles(angles);
			LuaClasses::VectorLua::push_vector(L, angles);
			return 1;
		}

		int SetViewAngles(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			if (vec == nullptr)
			{
				return 0;
			}

			interfaces::Engine->SetViewAngles(*vec);
			return 0;
		}

		int WorldToScreen(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			if (vec == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			Vector out; helper::engine::WorldToScreen(*vec, out);
			LuaClasses::VectorLua::push_vector(L, out);
			return 1;
		}
	}
}

namespace LuaFuncs
{
	namespace entitylist
	{
		const luaL_Reg entitylistlib[] =
		{
			{"GetAtIndex", GetEntityAtIndex},
			{"GetLocalPlayer", GetLocalPlayer},
			{"GetHighestIndex", GetHighestEntityIndex},
			{"GetPlayers", GetPlayers},
			{nullptr, nullptr}
		};

		void luaopen_entitylist(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, entitylistlib, 0);
			lua_setglobal(L, "entities");
		}

		int GetEntityAtIndex(lua_State* L)
		{
			if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
			{
				lua_pushnil(L);
				return 1;
			}

			int entindex = static_cast<int>(luaL_checkinteger(L, 1));
			CBaseEntity* entity = reinterpret_cast<CBaseEntity*>(interfaces::EntityList->GetClientEntity(entindex));
			if (entity == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::EntityLua::push_entity(L, entity);
			return 1;
		}

		int GetLocalPlayer(lua_State* L)
		{
			if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
			{
				lua_pushnil(L);
				return 1;
			}

			CTFPlayer* pLocal = EntityList::m_pLocalPlayer;
			if (pLocal == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::EntityLua::push_entity(L, pLocal);
			return 1;
		}

		int GetHighestEntityIndex(lua_State* L)
		{
			if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
			{
				lua_pushinteger(L, 0);
				return 1;
			}

			lua_pushinteger(L, interfaces::EntityList->GetHighestEntityIndex());
			return 1;
		}

		int GetPlayers(lua_State* L)
		{
			const auto& players = EntityList::m_vecPlayers;

			lua_newtable(L);

			int index = 1; // lua arrays start at 1

			for (CTFPlayer* player : players)
			{
				if (!player)
					continue;

				LuaClasses::EntityLua::push_entity(L, static_cast<CBaseEntity*>(player));
				lua_rawseti(L, -2, index++);
			}

			return 1; // return the table
		}
	}
}

namespace LuaFuncs
{
	namespace draw
	{
		HFont currentFont = 0;

		const luaL_Reg drawlib[]
		{
			{"Line", Line},
			{"FilledRect", FilledRect},
			{"FilledRectFade", FilledRectFade},
			{"FilledRectFadeFast", FilledRectFadeFast},
			{"OutlinedRect", OutlinedRect},
			{"SetColor", SetColor},
			{"SetTextColor", SetTextColor},
			{"GetTextSize", GetTextSize},
			{"OutlinedCircle", OutlinedCircle},
			{"Text", Text},
			{"CreateFont", CreateFont},
			{"SetFont", SetFont},
			{nullptr, nullptr},
		};

		void luaopen_draw(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, drawlib, 0);
			lua_setglobal(L, "draw");
		}

		int Line(lua_State* L)
		{
			int x0 = luaL_checkinteger(L, 1);
			int y0 = luaL_checkinteger(L, 2);
			int x1 = luaL_checkinteger(L, 3);
			int y1 = luaL_checkinteger(L, 4);

			interfaces::Surface->DrawLine(x0, y0, x1, y1);
			return 0;
		}

		int FilledRect(lua_State* L)
		{
			int x0 = luaL_checkinteger(L, 1);
			int y0 = luaL_checkinteger(L, 2);
			int x1 = luaL_checkinteger(L, 3);
			int y1 = luaL_checkinteger(L, 4);

			interfaces::Surface->DrawFilledRect(x0, y0, x1, y1);
			return 0;
		}

		int FilledRectFade(lua_State* L)
		{
			int x0 = luaL_checkinteger(L, 1);
			int y0 = luaL_checkinteger(L, 2);
			int x1 = luaL_checkinteger(L, 3);
			int y1 = luaL_checkinteger(L, 4);
			int alpha0 = luaL_checkinteger(L, 5);
			int alpha1 = luaL_checkinteger(L, 6);
			bool bHorizontal = luaL_checkinteger(L, 7);

			interfaces::Surface->DrawFilledRectFade(x0, y0, x1, y1, alpha0, alpha1, bHorizontal);
			return 0;
		}

		int FilledRectFadeFast(lua_State* L)
		{
			int x0 = luaL_checkinteger(L, 1);
			int y0 = luaL_checkinteger(L, 2);
			int x1 = luaL_checkinteger(L, 3);
			int y1 = luaL_checkinteger(L, 4);
			int fadeStartPt = luaL_checkinteger(L, 5);
			int fadeEndPt = luaL_checkinteger(L, 6);
			int alpha0 = luaL_checkinteger(L, 7);
			int alpha1 = luaL_checkinteger(L, 8);
			bool bHorizontal = luaL_checkinteger(L, 9);

			interfaces::Surface->DrawFilledRectFastFade(x0, y0, x1, y1, fadeStartPt, fadeEndPt, alpha0, alpha1, bHorizontal);
			return 0;
		}

		int OutlinedRect(lua_State* L)
		{
			int x0 = luaL_checkinteger(L, 1);
			int y0 = luaL_checkinteger(L, 2);
			int x1 = luaL_checkinteger(L, 3);
			int y1 = luaL_checkinteger(L, 4);

			interfaces::Surface->DrawOutlinedRect(x0, y0, x1, y1);
			return 0;
		}

		int SetColor(lua_State* L)
		{
			int r = luaL_checkinteger(L, 1);
			int g = luaL_checkinteger(L, 2);
			int b = luaL_checkinteger(L, 3);
			int a = luaL_optinteger(L, 4, 255);

			interfaces::Surface->DrawSetColor(r, g, b, a);
			return 0;
		}

		int SetTextColor(lua_State* L)
		{
			int r = luaL_checkinteger(L, 1);
			int g = luaL_checkinteger(L, 2);
			int b = luaL_checkinteger(L, 3);
			int a = luaL_optinteger(L, 4, 255);

			interfaces::Surface->DrawSetTextColor(r, g, b, a);
			return 0;
		}

		int GetTextSize(lua_State* L)
		{
			const char* str = luaL_checkstring(L, 1);
			if (str == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			// this is stupid
			std::string text(str);
			std::wstring wtext(text.begin(), text.end());
			
			int tw, th;
			interfaces::Surface->GetTextSize(currentFont, wtext.c_str(), tw, th);

			lua_pushnumber(L, tw);
			lua_pushnumber(L, th);
			return 2;
		}

		int OutlinedCircle(lua_State* L)
		{
			int x = luaL_checkinteger(L, 1);
			int y = luaL_checkinteger(L, 2);
			int radius = luaL_checkinteger(L, 3);
			int segments = luaL_checkinteger(L, 4);

			interfaces::Surface->DrawOutlinedCircle(x, y, radius, segments);
			return 0;
		}

		int Text(lua_State* L)
		{
			int x = luaL_checkinteger(L, 1);
			int y = luaL_checkinteger(L, 2);
			const char* str = luaL_checkstring(L, 3);

			std::string text(str);
			std::wstring wtext(text.begin(), text.end()); // this is fucking stupid

			interfaces::Surface->DrawSetTextPos(x, y);
			interfaces::Surface->DrawPrintText(wtext.c_str(), wtext.length());
			return 0;
		}

		int CreateFont(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			int height = luaL_checkinteger(L, 2);
			int weight = luaL_checkinteger(L, 3);
			int flags = luaL_checkinteger(L, 4);

			HFont font = interfaces::Surface->CreateFont();
			interfaces::Surface->SetFontGlyphSet(font, name, height, weight, 0, 0, flags);
			lua_pushinteger(L, font);
			return 1;
		}

		int SetFont(lua_State* L)
		{
			if (lua_isnoneornil(L, 1))
				return 0;

			int font = luaL_checkinteger(L, 1);
			interfaces::Surface->DrawSetTextFont(font);
			return 0;
		}
	}
}