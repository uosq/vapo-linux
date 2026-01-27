#include "funcs.h"
#include "../../gui/console.h"
#include <algorithm>
#include "api.h"
#include "pluto/lauxlib.h"
#include "pluto/lua.h"
#include "classes.h"
#include "hooks.h"
#include "../entitylist/entitylist.h"
#include "../../sdk/definitions/inetchannel.h"
#include "../../sdk/definitions/cclientstate.h"
#include "../../sdk/definitions/itexture.h"
#include "../../settings.h"
#include "../../imgui/imgui.h"
#include "../../sdk/definitions/ctracefilters.h"
#include "../../sdk/classes/weaponbase.h"
#include "../aimbot/utils/utils.h"
#include "../aimbot/aimbot.h"

namespace LuaFuncs
{
	namespace common
	{
		void luaopen_commonfunctions(lua_State* L)
		{
			lua_register(L, "print", &Print);
			lua_register(L, "typeof", &Typeof);
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

		const char* GetMetaName(lua_State* L, int idx)
		{
			if (!lua_getmetatable(L, idx))
				return nullptr; // no metatable

			lua_getfield(L, -1, "__name");

			const char* name = nullptr;
			if (lua_isstring(L, -1))
				name = lua_tostring(L, -1);

			lua_pop(L, 2); // pop __name and metatable
			return name;
		}

		int Typeof(lua_State* L)
		{
			const char* name = GetMetaName(L, 1);
			if (name == nullptr)
			{
				lua_pushstring(L, lua_typename(L, lua_type(L, 1)));
				return 1;
			}

			lua_pushstring(L, name);
			return 1;
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

			lua_pushboolean(L, LuaHookManager::Add(name, id, ref));
			return 0;
		}

		int Unregister(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			const char* id = luaL_checkstring(L, 2);

			lua_pushboolean(L, LuaHookManager::Remove(L, name, id));
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
			{"GetNetChannel", GetNetChannel},
			{"Trace", Trace},
			{"TraceHull", TraceHull},
			{"GetPointContents", GetPointContent},
			{"WorldToScreen", WorldToScreen},
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

			// never let it be from some random shit in the stack
			Vector out = {0, 0, 0};
			if (!helper::engine::WorldToScreen(*vec, out))
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::VectorLua::push_vector(L, out);
			return 1;
		}

		int GetNetChannel(lua_State* L)
		{
			CNetChannel* netchan = reinterpret_cast<CNetChannel*>(interfaces::Engine->GetNetChannelInfo());
			if (netchan == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::NetChannelLua::push_netchannel(Lua::m_luaState, netchan);
			return 1;
		}

		// engine.Trace(start, end, mask, shouldHitFunction(entity, contentsmask))
		int Trace(lua_State* L)
		{
			Vector* start = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			if (!start)
				return 0;

			Vector* end = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			if (!end)
				return 0;

			int mask = luaL_optinteger(L, 3, 0);

			Ray_t ray;
			ray.Init(*start, *end);

			CTraceFilterLua filter;
			CGameTrace trace;
			interfaces::EngineTrace->TraceRay(ray, mask, &filter, &trace);

			// warning:
			// big ass table below!
			lua_newtable(L);

			lua_pushinteger(L, trace.hitbox);
			lua_setfield(L, -2, "hitbox");

			lua_pushinteger(L, trace.hitgroup);
			lua_setfield(L, -2, "hitgroup");

			lua_pushnumber(L, trace.fractionleftsolid);
			lua_setfield(L, -2, "fractionleftsolid");

			lua_pushinteger(L, trace.contents);
			lua_setfield(L, -2, "contents");

			lua_pushboolean(L, trace.allsolid);
			lua_setfield(L, -2, "allsolid");

			lua_pushnumber(L, trace.fraction);
			lua_setfield(L, -2, "fraction");

			LuaClasses::VectorLua::push_vector(L, trace.plane.normal);
			lua_setfield(L, -2, "plane");

			if (trace.m_pEnt != nullptr)
			{
				LuaClasses::EntityLua::push_entity(L, trace.m_pEnt);
				lua_setfield(L, -2, "entity");
			}

			LuaClasses::VectorLua::push_vector(L, trace.startpos);
			lua_setfield(L, -2, "startpos");

			LuaClasses::VectorLua::push_vector(L, trace.endpos);
			lua_setfield(L, -2, "endpos");

			lua_pushinteger(L, trace.surface.surfaceProps);
			lua_setfield(L, -2, "surfaceProps");

			lua_pushinteger(L, trace.surface.flags);
			lua_setfield(L, -2, "surfaceFlags");

			lua_pushstring(L, trace.surface.name);
			lua_setfield(L, -2, "surfaceName");

			lua_pushinteger(L, trace.dispFlags);
			lua_setfield(L, -2, "dispFlags");

			lua_pushboolean(L, trace.startsolid);
			lua_setfield(L, -2, "startsolid");

			lua_pushboolean(L, trace.IsDispSurface());
			lua_setfield(L, -2, "IsDistSurface");

			lua_pushboolean(L, trace.IsDispSurfaceBuildable());
			lua_setfield(L, -2, "IsDistSurfaceBuildable");

			lua_pushboolean(L, trace.IsDispSurfaceProp1());
			lua_setfield(L, -2, "IsDistSurfaceProp1");

			lua_pushboolean(L, trace.IsDispSurfaceProp2());
			lua_setfield(L, -2, "IsDistSurfaceProp2");

			lua_pushboolean(L, trace.IsDispSurfaceWalkable());
			lua_setfield(L, -2, "IsDistSurfaceWalkable");

			return 1;
		}

		int TraceHull(lua_State* L)
		{
			Vector* start = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			if (!start)
				return 0;

			Vector* end = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			if (!end)
				return 0;

			Vector* min = static_cast<Vector*>(luaL_checkudata(L, 3, "Vector3"));
			if (!min)
				return 0;

			Vector* max = static_cast<Vector*>(luaL_checkudata(L, 4, "Vector3"));
			if (!max)
				return 0;

			int mask = luaL_optinteger(L, 5, 0);

			Ray_t ray;
			ray.Init(*start, *end, *min, *max);

			CTraceFilterLua filter;
			CGameTrace trace;
			interfaces::EngineTrace->TraceRay(ray, mask, &filter, &trace);

			// warning:
			// big ass table below!
			lua_newtable(L);

			lua_pushinteger(L, trace.hitbox);
			lua_setfield(L, -2, "hitbox");

			lua_pushinteger(L, trace.hitgroup);
			lua_setfield(L, -2, "hitgroup");

			lua_pushnumber(L, trace.fractionleftsolid);
			lua_setfield(L, -2, "fractionleftsolid");

			lua_pushinteger(L, trace.contents);
			lua_setfield(L, -2, "contents");

			lua_pushboolean(L, trace.allsolid);
			lua_setfield(L, -2, "allsolid");

			lua_pushnumber(L, trace.fraction);
			lua_setfield(L, -2, "fraction");

			LuaClasses::VectorLua::push_vector(L, trace.plane.normal);
			lua_setfield(L, -2, "plane");

			if (trace.m_pEnt != nullptr)
			{
				LuaClasses::EntityLua::push_entity(L, trace.m_pEnt);
				lua_setfield(L, -2, "entity");
			}

			LuaClasses::VectorLua::push_vector(L, trace.startpos);
			lua_setfield(L, -2, "startpos");

			LuaClasses::VectorLua::push_vector(L, trace.endpos);
			lua_setfield(L, -2, "endpos");

			lua_pushinteger(L, trace.surface.surfaceProps);
			lua_setfield(L, -2, "surfaceProps");

			lua_pushinteger(L, trace.surface.flags);
			lua_setfield(L, -2, "surfaceFlags");

			lua_pushstring(L, trace.surface.name);
			lua_setfield(L, -2, "surfaceName");

			lua_pushinteger(L, trace.dispFlags);
			lua_setfield(L, -2, "dispFlags");

			lua_pushboolean(L, trace.startsolid);
			lua_setfield(L, -2, "startsolid");

			lua_pushboolean(L, trace.IsDispSurface());
			lua_setfield(L, -2, "IsDistSurface");

			lua_pushboolean(L, trace.IsDispSurfaceBuildable());
			lua_setfield(L, -2, "IsDistSurfaceBuildable");

			lua_pushboolean(L, trace.IsDispSurfaceProp1());
			lua_setfield(L, -2, "IsDistSurfaceProp1");

			lua_pushboolean(L, trace.IsDispSurfaceProp2());
			lua_setfield(L, -2, "IsDistSurfaceProp2");

			lua_pushboolean(L, trace.IsDispSurfaceWalkable());
			lua_setfield(L, -2, "IsDistSurfaceWalkable");

			return 1;
		}

		int GetPointContent(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			if (!vec)
				return 0;

			lua_pushinteger(L, interfaces::EngineTrace->GetPointContents(*vec));
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
			{"GetTeammates", GetTeammates},
			{"GetEnemies", GetEnemies},
			{"GetCurrentWeapon", GetCurrentWeapon},
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

		int GetTeammates(lua_State* L)
		{
			const auto& teammates = EntityList::m_vecTeammates;

			lua_newtable(L);

			int index = 1; // lua arrays start at 1

			for (const auto& team : teammates)
			{
				if (!team)
					continue;

				LuaClasses::EntityLua::push_entity(L, static_cast<CBaseEntity*>(team));
				lua_rawseti(L, -2, index++);
			}

			return 1; // return the table
		}

		int GetEnemies(lua_State* L)
		{
			const auto& enemies = EntityList::m_vecEnemies;

			lua_newtable(L);

			int index = 1; // lua arrays start at 1

			for (const auto& enemy : enemies)
			{
				if (!enemy)
					continue;

				LuaClasses::EntityLua::push_entity(L, static_cast<CBaseEntity*>(enemy));
				lua_rawseti(L, -2, index++);
			}

			return 1; // return the table
		}

		int GetCurrentWeapon(lua_State* L)
		{
			if (!interfaces::Engine->IsInGame() || !interfaces::Engine->IsConnected())
				return 0;

			CTFPlayer* pLocal = EntityList::m_pLocalPlayer;
			if (pLocal == nullptr)
				return 0;

			CTFWeaponBase* pWeapon = HandleAs<CTFWeaponBase*>(pLocal->GetActiveWeapon());
			if (pWeapon == nullptr)
				return 0;

			LuaClasses::EntityLua::push_entity(L, pWeapon);
			return 1;
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
			int flags = luaL_optinteger(L, 4, EFONTFLAG_CUSTOM | EFONTFLAG_ANTIALIAS);

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
			currentFont = font;
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace render
	{
		const luaL_Reg renderlib[]
		{
			{"GetColorModulation", GetColorModulation},
			{"SetColorModulation", SetColorModulation},
			{"GetBlend", GetBlend},
			{"SetBlend", SetBlend},
			{"ForcedMaterialOverride", ForcedMaterialOverride},
			{"GetMaterialOverride", GetMaterialOverride},
			{"DrawScreenSpaceRectangle", DrawScreenSpaceRectangle},
			{"DrawScreenSpaceQuad", DrawScreenSpaceQuad},
			{"GetViewport", GetViewport},
			{"DepthRange", DepthRange},
			{"GetDepthRange", GetDepthRange},
			{"SetRenderTarget", SetRenderTarget},
			{"GetRenderTarget", GetRenderTarget},
			{"ClearBuffers", ClearBuffers},
			{"ClearColor3ub", ClearColor3ub},
			{"ClearColor4ub", ClearColor4ub},
			{"OverrideDepthEnable", OverrideDepthEnable},
			{"OverrideAlphaEnable", OverrideAlphaEnable},
			{"OverrideColorEnable", OverrideColorEnable},
			{"PushRenderTargetAndViewport", PushRenderTargetAndViewport},
			{"PopRenderTargetAndViewport", PopRenderTargetAndViewport},
			{"SetStencilEnable", SetStencilEnable},
			{"SetStencilFailOperation", SetStencilFailOperation},
			{"SetStencilZFailOperation", SetStencilZFailOperation},
			{"SetStencilPassOperation", SetStencilPassOperation},
			{"SetStencilCompareFunction", SetStencilCompareFunction},
			{"SetStencilReferenceValue", SetStencilReferenceValue},
			{"SetStencilTestMask", SetStencilTestMask},
			{"SetStencilWriteMask", SetStencilWriteMask},
			{"ClearStencilBufferRectangle", ClearStencilBufferRectangle},
			{"Viewport", Viewport},
			{nullptr, nullptr}
		};

		void luaopen_render(lua_State *L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, renderlib, 0);
			lua_setglobal(L, "render");
		}

		int GetColorModulation(lua_State* L)
		{
			float color[3] = {0, 0, 0};
			interfaces::RenderView->GetColorModulation(color);

			lua_pushnumber(L, color[0]);
			lua_pushnumber(L, color[1]);
			lua_pushnumber(L, color[2]);
			return 3;
		}

		int SetColorModulation(lua_State* L)
		{
			float r = luaL_optnumber(L, 1, 0.0f);
			float g = luaL_optnumber(L, 2, 0.0f);
			float b = luaL_optnumber(L, 3, 0.0f);

			r = std::clamp(r, 0.0f, 1.0f);
			g = std::clamp(g, 0.0f, 1.0f);
			b = std::clamp(b, 0.0f, 1.0f);

			float color[3] = {r, g, b};

			interfaces::RenderView->SetColorModulation(color);
			return 0;
		}

		int GetBlend(lua_State* L)
		{
			lua_pushnumber(L, interfaces::RenderView->GetBlend());
			return 1;
		}

		int SetBlend(lua_State* L)
		{
			float blend = luaL_optnumber(L, 1, 0.0f);
			interfaces::RenderView->SetBlend(blend);
			return 0;
		}

		int ForcedMaterialOverride(lua_State *L)
		{
			if (lua_isnil(L, 1))
			{
				interfaces::ModelRender->ForcedMaterialOverride(nullptr);
				return 0;
			}

			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			interfaces::ModelRender->ForcedMaterialOverride(lmat->mat);
			return 0;
		}

		int GetMaterialOverride(lua_State *L)
		{
			IMaterial* mat;
			OverrideType_t override;
			interfaces::ModelRender->GetMaterialOverride(&mat, &override);
			LuaClasses::MaterialLua::push_material(L, mat);
			return 1;
		}

		int DrawScreenSpaceRectangle(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
			{
				luaL_error(L, "Expected material, got nil!");
				return 0;
			}

			int destx = luaL_checkinteger(L, 2);
			int desty = luaL_checkinteger(L, 3);
			int width = luaL_checkinteger(L, 4);
			int height = luaL_checkinteger(L, 5);
			float src_texture_x0 = luaL_checknumber(L, 6);
			float src_texture_y0 = luaL_checknumber(L, 7);
			float src_texture_x1 = luaL_checknumber(L, 8);
			float src_texture_y1 = luaL_checknumber(L, 9);
			int src_texture_width = luaL_checkinteger(L, 10);
			int src_texture_height = luaL_checkinteger(L, 11);

			pRenderContext->DrawScreenSpaceRectangle(lmat->mat, destx, desty, width, height, src_texture_x0, src_texture_y0, src_texture_x1, src_texture_y1, src_texture_width, src_texture_height);
			return 0;
		}

		int DrawScreenSpaceQuad(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
			{
				luaL_error(L, "Expected material, got nil!");
				return 0;
			}

			pRenderContext->DrawScreenSpaceQuad(lmat->mat);
			return 0;
		}

		int GetViewport(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int x, y, width, height;
			pRenderContext->GetViewport(x, y, width, height);

			lua_pushinteger(L, x);
			lua_pushinteger(L, y);
			lua_pushinteger(L, width);
			lua_pushinteger(L, height);
			return 4;
		}

		int DepthRange(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			float zNear = 0.0f, zFar = 1.0f;
			zNear = luaL_checknumber(L, 1);
			zFar = luaL_checknumber(L, 2);

			pRenderContext->DepthRange(zNear, zFar);
			return 0;
		}

		int GetDepthRange(lua_State* L)
		{
			CViewSetup view;
			if (!interfaces::ClientDLL->GetPlayerView(view))
			{
				lua_pushnil(L);
				lua_pushnil(L);
				return 2;
			}

			lua_pushinteger(L, view.zNear);
			lua_pushinteger(L, view.zFar);
			return 2;
		}

		int SetRenderTarget(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Expected Texture, received nil");
				return 0;
			}

			if (!ltex->tex->IsRenderTarget())
			{
				luaL_error(L, "Expected a Render Target Texture, received normal Texture (Did you use the wrong one?)");
				return 0;
			}

			pRenderContext->SetRenderTarget(ltex->tex);
			return 0;
		}

		int GetRenderTarget(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			ITexture* rendertarget = pRenderContext->GetRenderTarget();
			if (rendertarget == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::TextureLua::push_texture(L, rendertarget);
			return 1;
		}

		int ClearBuffers(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			bool bClearColor = luaL_checkinteger(L, 1);
			bool bClearDepth = luaL_checkinteger(L, 2);
			bool bClearStencil = luaL_optinteger(L, 3, 0);

			pRenderContext->ClearBuffers(bClearColor, bClearDepth, bClearStencil);

			return 0;
		}

		int ClearColor3ub(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			unsigned char r = static_cast<unsigned char>(luaL_checkinteger(L, 1));
			unsigned char g = static_cast<unsigned char>(luaL_checkinteger(L, 2));
			unsigned char b = static_cast<unsigned char>(luaL_checkinteger(L, 3));

			pRenderContext->ClearColor3ub(r, g, b);
			return 0;
		}

		int ClearColor4ub(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			unsigned char r = static_cast<unsigned char>(luaL_checkinteger(L, 1));
			unsigned char g = static_cast<unsigned char>(luaL_checkinteger(L, 2));
			unsigned char b = static_cast<unsigned char>(luaL_checkinteger(L, 3));
			unsigned char a = static_cast<unsigned char>(luaL_checkinteger(L, 4));

			pRenderContext->ClearColor4ub(r, g, b, a);
			return 0;
		}

		int OverrideDepthEnable(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			bool bEnable = luaL_checkinteger(L, 1);
			bool bDepthEnable = luaL_checkinteger(L, 2);
			pRenderContext->OverrideDepthEnable(bEnable, bDepthEnable);

			return 0;
		}

		int OverrideAlphaEnable(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			bool bEnable = luaL_checkinteger(L, 1);
			bool bAlphaWriteEnable = luaL_checkinteger(L, 2);

			pRenderContext->OverrideAlphaWriteEnable(bEnable, bAlphaWriteEnable);
			return 0;
		}

		int OverrideColorEnable(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			bool bEnable = luaL_checkinteger(L, 1);
			bool bColorWriteEnable = luaL_checkinteger(L, 2);

			pRenderContext->OverrideColorWriteEnable(bEnable, bColorWriteEnable);
			return 0;
		}

		int PushRenderTargetAndViewport(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			pRenderContext->PushRenderTargetAndViewport();
			return 0;
		}

		int PopRenderTargetAndViewport(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			pRenderContext->PopRenderTargetAndViewport();
			return 0;
		}

		int SetStencilEnable(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			bool state = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilEnable(state);
			return 0;
		}

		int SetStencilFailOperation(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int op = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilFailOperation(static_cast<StencilOperation_t>(op));
			return 0;
		}

		int SetStencilZFailOperation(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int op = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilZFailOperation(static_cast<StencilOperation_t>(op));
			return 0;
		}

		int SetStencilPassOperation(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int op = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilPassOperation(static_cast<StencilOperation_t>(op));
			return 0;
		}

		int SetStencilCompareFunction(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int op = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilCompareFunction(static_cast<StencilComparisonFunction_t>(op));
			return 0;
		}

		int SetStencilReferenceValue(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int ref = luaL_checkinteger(L, 1);
			pRenderContext->SetStencilReferenceValue(ref);
			return 0;
		}

		int SetStencilTestMask(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			uint32_t mask = static_cast<uint32_t>(luaL_checkinteger(L, 1));
			pRenderContext->SetStencilTestMask(mask);
			return 0;
		}

		int SetStencilWriteMask(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			uint32_t mask = static_cast<uint32_t>(luaL_checkinteger(L, 1));
			pRenderContext->SetStencilWriteMask(mask);
			return 0;
		}

		int ClearStencilBufferRectangle(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int xmin = luaL_checkinteger(L, 1);
			int ymin = luaL_checkinteger(L, 2);
			int xmax = luaL_checkinteger(L, 3);
			int ymax = luaL_checkinteger(L, 4);
			int value = luaL_checkinteger(L, 5);

			pRenderContext->ClearStencilBufferRectangle(xmin, ymin, xmax, ymax, value);
			return 0;
		}

		int Viewport(lua_State* L)
		{
			auto pRenderContext = interfaces::MaterialSystem->GetRenderContext();
			if (pRenderContext == nullptr)
			{
				luaL_error(L, "Render context is nil!");
				return 0;
			}

			int x = luaL_checkinteger(L, 1);
			int y = luaL_checkinteger(L, 2);
			int width = luaL_checkinteger(L, 3);
			int height = luaL_checkinteger(L, 4);
			pRenderContext->Viewport(x, y, width, height);
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace materials
	{
		const luaL_Reg matslib[]
		{
			{"Create", Create},
			{"FindMaterial", FindMaterial},
			{"FindTexture", FindTexture},
			{"CreateTextureRenderTarget", CreateTextureRenderTarget},
			//{"Enumerate", Enumerate},
			{nullptr, nullptr}
		};

		void luaopen_materials(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, matslib, 0);
			lua_setglobal(L, "materials");
		}

		// materials.Create("name", "vmt")
		int Create(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			if (name == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* vmt = luaL_checkstring(L, 2);
			if (vmt == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			IMaterial* mat = helper::material::CreateMaterial(name, vmt);
			if (mat == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}
			
			LuaClasses::MaterialLua::push_material(L, mat);
			return 1;
		}

		int FindMaterial(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			const char* groupName = luaL_checkstring(L, 2);
			bool complain = luaL_optinteger(L, 3, 0);
			IMaterial* mat = interfaces::MaterialSystem->FindMaterial(name, groupName, complain);
			if (mat == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::MaterialLua::push_material(L, mat);
			return 1;
		}

		int FindTexture(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			const char* groupName = luaL_checkstring(L, 2);
			bool complain = luaL_optinteger(L, 3, 1);

			ITexture* tex = interfaces::MaterialSystem->FindTexture(name, groupName, complain);
			if (tex == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::TextureLua::push_texture(L, tex);
			return 1;
		}

		int CreateTextureRenderTarget(lua_State* L)
		{
			const char* pRTName = luaL_checkstring(L, 1);
			int w = luaL_checkinteger(L, 2);
			int h = luaL_checkinteger(L, 3);

			ITexture* tex = interfaces::MaterialSystem->CreateNamedRenderTargetTexture(pRTName, w, h, RenderTargetSizeMode_t::RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888);
			if (tex != nullptr)
			{
				LuaClasses::TextureLua::push_texture(L, tex);
				return 1;
			}

			lua_pushnil(L);
			return 1;
		}

		// materials.Enumerate(function(mat: Material) end)
		int Enumerate(lua_State* L)
		{
			luaL_checktype(L, 1, LUA_TFUNCTION);

			lua_pushvalue(L, 1);
			int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);

			MaterialHandle_t invalidMaterial = interfaces::MaterialSystem->InvalidMaterial();

			for (auto i = interfaces::MaterialSystem->FirstMaterial(); i != invalidMaterial; i = interfaces::MaterialSystem->NextMaterial(i))
			{
				IMaterial* mat = interfaces::MaterialSystem->GetMaterial(i);
				if (mat == nullptr)
					continue;

				lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);

				LuaClasses::MaterialLua::push_material(L, mat);

				if (lua_pcall(L, 1, 0, 0) != LUA_OK)
				{
					lua_tostring(L, -1); // get error
					lua_pop(L, 1); // pop error
				}
			}

			luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace client
	{
		const luaL_Reg clientlib[]
		{
			{"GetConVar", GetConVar},
			{"SetConVar", SetConVar},
			{"ChatSay", ChatSay},
			{"Command", Command},
			{"IsClassMenuOpen", IsClassMenuOpen},
			{"ClearConsole", ClearConsole},
			//{"ChatPrintf", ChatPrintf}, Why this not work?? :sob:
			//{"Notification", Notification},
			{nullptr, nullptr}
		};

		void luaopen_client(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, clientlib, 0);
			lua_setglobal(L, "client");
		}

		int GetConVar(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs != 1)
			{
				luaL_error(L, "Expected 1 argument! Received %d", nargs);
				return 0;
			}

			if (!lua_isstring(L, 1))
			{
				luaL_error(L, "Expected 1st argument as string!");
				return 0;
			}

			ConVar* cvar = interfaces::Cvar->FindVar(luaL_checkstring(L, 1));
			if (cvar == nullptr)
			{
				luaL_error(L, "Couldn't find ConVar!");
				return 0;
			}

			lua_pushinteger(L, cvar->GetInt());
			lua_pushnumber(L, cvar->GetFloat());
			lua_pushstring(L, cvar->GetString());
			return 3;
		}

		int SetConVar(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs != 2)
			{
				luaL_error(L, "Expected 2 arguments! Received %d", nargs);
				return 0;
			}

			if (!lua_isstring(L, 1))
			{
				luaL_error(L, "Expected 1st argument as string!");
				return 0;
			}

			if (lua_isnoneornil(L, 2))
			{
				luaL_error(L, "Argument #2 is nil!");
				return 0;
			}

			ConVar* cvar = interfaces::Cvar->FindVar(luaL_checkstring(L, 1));
			if (cvar == nullptr)
			{
				luaL_error(L, "Couldn't find ConVar!");
				return 0;
			}

			if (lua_isinteger(L, 2))
				cvar->SetValue(static_cast<int>(luaL_checkinteger(L, 2)));
			else if (lua_isnumber(L, 2))
				cvar->SetValue(static_cast<float>(luaL_checknumber(L, 2)));
			else if (lua_isstring(L, 2))
				cvar->SetValue(luaL_checkstring(L, 2));

			return 0;
		}

		int ChatSay(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs != 1)
			{
				luaL_error(L, "Expected 1 argument! Received %s", nargs);
				return 0;
			}

			const char* text = luaL_checkstring(L, 1);
			if (text == nullptr)
			{
				luaL_error(L, "Text is a null pointer!");
				return 0;
			}

			interfaces::Engine->ClientCmd((std::string("say ") + text).c_str());
			return 0;
		}

		int Command(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs == 0 || nargs > 2)
			{
				luaL_error(L, "Expected at least 1 argument! Received %s", nargs);
				return 0;
			}

			if (!lua_isstring(L, 1))
			{
				luaL_error(L, "Expected 1st argument as string!");
				return 0;
			}

			if (lua_istrue(L, 2))
				interfaces::Engine->ClientCmd_Unrestricted(luaL_checkstring(L, 1));
			else
				interfaces::Engine->ClientCmd(luaL_checkstring(L, 1));

			return 0;
		}

		int IsClassMenuOpen(lua_State* L)
		{
			static ConVar* _cl_classmenuopen = interfaces::Cvar->FindVar("_cl_classmenuopen");
			if (_cl_classmenuopen == nullptr)
				return 0;

			lua_pushboolean(L, _cl_classmenuopen->GetInt());
			return 1;
		}

		int ChatPrintf(lua_State* L)
		{
			int nargs = lua_gettop(L);
			if (nargs == 0)
				return 0;

			for (int i = 1; i <= nargs; i++)
			{
				const char* str = luaL_tolstring(L, i, nullptr);
				if (str) helper::localplayer::ChatPrintf(0, CHAT_FILTER_NONE, str);
			}

			return 0;
		}

		/*int Notification(lua_State* L)
		{
			CEconNotification* notification = new CEconNotification();
			notification->SetText(luaL_checkstring(L, 1));
			notification->SetLifetime(999.0f);
			interfaces::g_notificationQueue->AddNotification(notification);
			return 0;
		}*/

		int ClearConsole(lua_State *L)
		{
			interfaces::EngineVGui->ClearConsole();
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace clientstate
	{
		const luaL_Reg clientstatelib[]
		{
			{"GetChokedCommands", GetChokedCommands},
			{"GetLastOutgoingCommand", GetLastOutgoingCommand},
			{"GetClientSignonState", GetClientSignonState},
			{"GetDeltaTick", GetDeltaTick},
			{"GetLastCommandAck", GetLastCommandAck},
			{"ForceFullUpdate", ForceFullUpdate},
			{nullptr, nullptr}
		};

		void luaopen_clientstate(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, clientstatelib, 0);
			lua_setglobal(L, "clientstate");
		}

		int GetChokedCommands(lua_State* L)
		{
			if (interfaces::ClientState == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CClientState*>(interfaces::ClientState)->chokedcommands);
			return 1;
		}

		int GetLastOutgoingCommand(lua_State* L)
		{
			if (interfaces::ClientState == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CClientState*>(interfaces::ClientState)->lastoutgoingcommand);
			return 1;
		}

		int GetClientSignonState(lua_State* L)
		{
			if (interfaces::ClientState == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CClientState*>(interfaces::ClientState)->m_nSignonState);
			return 1;
		}

		int GetDeltaTick(lua_State* L)
		{
			if (interfaces::ClientState == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CClientState*>(interfaces::ClientState)->m_nDeltaTick);
			return 1;
		}

		int GetLastCommandAck(lua_State* L)
		{
			if (interfaces::ClientState == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CClientState*>(interfaces::ClientState)->last_command_ack);
			return 1;
		}

		int ForceFullUpdate(lua_State *L)
		{
			// xref: Requesting full game update
			if (interfaces::ClientState == nullptr)
				return 0;

			// I should probably add this to the clientstate def
			// But I don't use it anywhere in the code sooo

			using ForceFullUpdateFn = void(*)(CClientState* self);
			static ForceFullUpdateFn original_ForceFullUpdate = reinterpret_cast<ForceFullUpdateFn>(sigscan_module("engine.so", "83 BF B8 01 00 00 FF 74 ? 55"));
			if (original_ForceFullUpdate == nullptr)
				return 0;

			original_ForceFullUpdate(reinterpret_cast<CClientState*>(interfaces::ClientState));
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace input
	{
		const luaL_Reg inputlib[]
		{
			{"IsButtonDown", IsButtonDown},
			{"IsButtonPressed", IsButtonPressed},
			{"IsButtonReleased", IsButtonReleased},
			{"SetCursorAlwaysVisible", SetCursorAlwaysVisible},
			{"IsCursorVisible", IsCursorVisible},
			{"GetPollTick", GetPollTick},
			{nullptr, nullptr}
		};

		void luaopen_input(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, inputlib, 0);
			lua_setglobal(L, "input");
		}

		int IsButtonDown(lua_State* L)
		{
			int btn = luaL_checkinteger(L, 1);
			ButtonCode_t button = static_cast<ButtonCode_t>(btn);
			if (button == BUTTON_CODE_INVALID)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			lua_pushboolean(L, interfaces::InputSystem->IsButtonDown(button));
			return 1;
		}

		int IsButtonPressed(lua_State* L)
		{
			int btn = luaL_checkinteger(L, 1);
			ButtonCode_t button = static_cast<ButtonCode_t>(btn);
			if (button == BUTTON_CODE_INVALID)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			bool down = interfaces::InputSystem->IsButtonDown(button);
			int tick = interfaces::InputSystem->GetButtonPressedTick(button);
			lua_pushboolean(L, down);
			lua_pushinteger(L, tick);
			return 2;
		}

		int IsButtonReleased(lua_State* L)
		{
			int btn = luaL_checkinteger(L, 1);
			ButtonCode_t button = static_cast<ButtonCode_t>(btn);
			if (button == BUTTON_CODE_INVALID)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			bool down = interfaces::InputSystem->IsButtonDown(button);
			int tick = interfaces::InputSystem->GetButtonReleasedTick(button);
			lua_pushboolean(L, down);
			lua_pushinteger(L, tick);
			return 2;
		}

		int SetCursorAlwaysVisible(lua_State* L)
		{
			bool state = luaL_checkinteger(L, 1);
			interfaces::Surface->SetCursorAlwaysVisible(state);
			return 0;
		}

		int IsCursorVisible(lua_State* L)
		{
			lua_pushboolean(L, interfaces::Surface->IsCursorVisible());
			return 1;
		}

		int GetCursorPos(lua_State* L)
		{
			int x, y;
			interfaces::Surface->SurfaceGetCursorPos(x, y);
			lua_pushinteger(L, x);
			lua_pushinteger(L, y);
			return 2;
		}

		int GetPollTick(lua_State* L)
		{
			lua_pushinteger(L, interfaces::InputSystem->GetPollTick());
			return 1;
		}
	}
}

namespace LuaFuncs
{
	namespace menu
	{
		const luaL_Reg menulib[]
		{
			{"IsOpen", IsOpen},
			{"SetOpen", SetOpen},
			{"SetValue", SetValue},
			{"GetValue", GetValue},
			{nullptr, nullptr}
		};

		int SetValue(lua_State* L)
		{
			const char* key = luaL_checkstring(L, 1);
			auto it = g_SettingsMap.find(key);
			if (it == g_SettingsMap.end())
				return 0;

			SettingEntry& e = it->second;

			switch (e.type)
			{
				case SettingType::BOOL:
				*reinterpret_cast<bool*>(e.ptr) = lua_toboolean(L, 2);
				break;

				case SettingType::INT:
				*reinterpret_cast<int*>(e.ptr) = luaL_checkinteger(L, 2);
				break;

				case SettingType::FLOAT:
				*reinterpret_cast<float*>(e.ptr) = (float)luaL_checknumber(L, 2);
				break;

				case SettingType::STRING:
				*reinterpret_cast<std::string*>(e.ptr) = luaL_checkstring(L, 2);
				break;
			}

			return 0;
		}

		int GetValue(lua_State* L)
		{
			const char* key = luaL_checkstring(L, 1);
			auto it = g_SettingsMap.find(key);
			if (it == g_SettingsMap.end())
				return 0;

			SettingEntry& e = it->second;

			switch (e.type)
			{
				case SettingType::BOOL:
				lua_pushboolean(L, *reinterpret_cast<bool*>(e.ptr));
				break;

				case SettingType::INT:
				lua_pushinteger(L, *reinterpret_cast<bool*>(e.ptr));
				break;

				case SettingType::FLOAT:
				lua_pushnumber(L, *reinterpret_cast<bool*>(e.ptr));
				break;

				case SettingType::STRING:
				lua_pushstring(L, (reinterpret_cast<std::string*>(e.ptr)->c_str()));
				break;
			}

			return 1;
		}

		void luaopen_menu(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, menulib, 0);
			lua_setglobal(L, "menu");

			RegisterSettings();
		}

		int IsOpen(lua_State* L)
		{
			lua_pushboolean(L, settings.menu_open);
			return 1;
		}

		int SetOpen(lua_State* L)
		{
			int open = luaL_checkinteger(L, 1);
			settings.menu_open = open;
			interfaces::Surface->SetCursorAlwaysVisible(open);
			return 0;
		}
	}
}

namespace LuaFuncs
{
	namespace ui
	{
		const luaL_Reg uilib[]
		{
			{"Begin", Begin},
			{"Button", Button},
			{"Checkbox", Checkbox},
			{"TextUnformatted", TextUnformatted},
			{"SliderFloat", SliderFloat},
			{"End", End},
			{nullptr, nullptr}
		};

		void luaopen_ui(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, uilib, 0);
			lua_setglobal(L, "ui");
		}
		
		int Begin(lua_State* L)
		{
			const char* name = luaL_checkstring(L, 1);
			if (strcmp(name, "Skill Issue") == 0)
			{
				luaL_error(L, "Can't use the menu's name!");
				return 0;
			}

			int flags = luaL_optinteger(L, 2, 0);

			lua_pushboolean(L, ImGui::Begin(name, nullptr, flags));
			return 1;
		}

		int Button(lua_State* L)
		{
			const char* label = luaL_checkstring(L, 1);
			lua_pushboolean(L, ImGui::Button(label));
			return 1;
		}

		int Checkbox(lua_State* L)
		{
			const char* label = luaL_checkstring(L, 1);
			luaL_checktype(L, 2, LUA_TBOOLEAN);

			bool value = lua_toboolean(L, 2);

			bool changed = ImGui::Checkbox(label, &value);

			lua_pushboolean(L, changed);
			lua_pushboolean(L, value);
			return 2;
		}

		int TextUnformatted(lua_State* L)
		{
			const char* text = luaL_checkstring(L, 1);
			ImGui::TextUnformatted(text);
			return 1;
		}

		int SliderFloat(lua_State* L)
		{
			const char* label = luaL_checkstring(L, 1);
			float value = luaL_checknumber(L, 2);
			float min = luaL_checknumber(L, 3);
			float max = luaL_checknumber(L, 4);
			lua_pushboolean(L, ImGui::SliderFloat(label, &value, min, max));
			lua_pushnumber(L, value);
			return 2;
		}

		int End(lua_State* L)
		{
			ImGui::End();
			return 1;
		}
	}
}

namespace LuaFuncs
{
	namespace aimbot
	{
		const luaL_Reg aimbotlib[]
		{
			{"GetTarget", GetTarget},
			{"SetTarget", SetTarget},
			{"GetMode", GetMode},
			{"SetMode", SetMode},
			{"GetFOV", GetFOV},
			{"SetFOV", SetFOV},
			{"IsEntityValid", IsEntityValid},
			{"IsRunning", IsRunning},
			{"GetKey", GetKey},
			{"SetKey", SetKey},
			{nullptr, nullptr}
		};

		void luaopen_aimbot(lua_State* L)
		{
			lua_newtable(L);
			luaL_setfuncs(L, aimbotlib, 0);
			lua_setglobal(L, "aimbot");
		}

		int GetTarget(lua_State* L)
		{
			CBaseEntity* pTarget = EntityList::m_pAimbotTarget;
			if (pTarget == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaClasses::EntityLua::push_entity(L, pTarget);
			return 1;
		}

		int SetTarget(lua_State* L)
		{
			if (lua_isnoneornil(L, 1))
			{
				EntityList::m_pAimbotTarget = nullptr;
				return 0;
			}

			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
			{
				EntityList::m_pAimbotTarget = nullptr;
				return 0;
			}

			EntityList::m_pAimbotTarget = le->ent;
			return 0;
		}

		int GetMode(lua_State* L)
		{
			lua_pushinteger(L, int(settings.aimbot.mode));
			lua_pushstring(L, AimbotUtils::GetAimbotModeName().c_str());
			return 2;
		}

		int SetMode(lua_State* L)
		{
			int mode = luaL_checkinteger(L, 1);
			if (int(AimbotMode::INVALID) >= mode || int(AimbotMode::MAX) <= mode )
			{
				luaL_error(L, "Aimbot mode must be in a valid range!");
				return 0;
			}

			settings.aimbot.mode = AimbotMode(mode);
			return 0;
		}

		int GetFOV(lua_State* L)
		{
			lua_pushnumber(L, settings.aimbot.fov);
			return 1;
		}

		int SetFOV(lua_State* L)
		{
			float fov = luaL_checknumber(L, 1);
			if (fov > 180.0f || fov < 0.0f)
			{
				luaL_error(L, "Aimbot FOV must be in a valid range! (0, 180)");
				return 0;
			}

			return 0;
		}

		int IsEntityValid(lua_State* L)
		{
			if (lua_isnoneornil(L, 1))
			{
				lua_pushboolean(L, false);
				return 1;
			}

			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			const auto& pLocal = EntityList::m_pLocalPlayer;
			if (pLocal == nullptr)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			lua_pushboolean(L, AimbotUtils::IsValidEntity(le->ent, pLocal->m_iTeamNum()));
			return 1;
		}

		int IsRunning(lua_State* L)
		{
			lua_pushboolean(L, Aimbot::state.running);
			return 1;
		}

		int GetKey(lua_State* L)
		{
			const std::string& key = settings.aimbot.key;
			ButtonCode_t btn = interfaces::InputSystem->StringToButtonCode(key.c_str());

			if (!helper::input::IsButtonValid(btn))
			{
				lua_pushnil(L);
				lua_pushnil(L);
				return 2;
			}

			lua_pushinteger(L, static_cast<lua_Integer>(btn));
			lua_pushstring(L, key.c_str());
			return 2;
		}

		int SetKey(lua_State* L)
		{
			if (lua_isnoneornil(L, 1))
				return 0;

			if (lua_isstring(L, 1))
			{
				const char* str = luaL_checkstring(L, 1);
				ButtonCode_t btn = interfaces::InputSystem->StringToButtonCode(str);

				if (helper::input::IsButtonValid(btn))
					settings.aimbot.key = str;

				return 0;
			}

			if (lua_isinteger(L, 1))
			{
				const int key = luaL_checkinteger(L, 1);
				ButtonCode_t btn = interfaces::InputSystem->VirtualKeyToButtonCode(key);
				
				if (helper::input::IsButtonValid(btn))
					settings.aimbot.key = interfaces::InputSystem->ButtonCodeToString(btn);

				return 0;
			}

			return 0;
		}
	}
}