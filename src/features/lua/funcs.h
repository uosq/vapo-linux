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

		extern const luaL_Reg globalvarslib[];
	}

	namespace engine
	{
		void luaopen_engine(lua_State* L);
		int IsInGame(lua_State* L);
		int IsConnected(lua_State* L);
		int IsTakingScreenshot(lua_State* L);
		int IsGameUIVisible(lua_State* L);
		int IsConsoleVisible(lua_State* L);
		int SetViewAngles(lua_State* L);
		int GetViewAngles(lua_State* L);
		int WorldToScreen(lua_State* L);

		extern const luaL_Reg enginelib[];
	}

	namespace hooks
	{
		void luaopen_hooks(lua_State* L);
		int Register(lua_State* L);
		int Unregister(lua_State* L);

		extern const luaL_Reg hooklib[];
	}

	namespace entitylist
	{
		extern const luaL_Reg entitylistlib[];

		void luaopen_entitylist(lua_State* L);
		int GetEntityAtIndex(lua_State* L);
		int GetLocalPlayer(lua_State* L);
		int GetHighestEntityIndex(lua_State* L);
		int GetPlayers(lua_State* L);
		int GetPlayerResources(lua_State* L);
		int GetTeammates(lua_State* L);
		int GetEnemies(lua_State* L);
	}

	namespace draw
	{
		extern const luaL_Reg drawlib[];
		extern HFont currentFont;

		void luaopen_draw(lua_State* L);
		int Line(lua_State* L);
		int FilledRect(lua_State* L);
		int FilledRectFade(lua_State* L);
		int FilledRectFadeFast(lua_State* L);
		int OutlinedRect(lua_State* L);
		int SetColor(lua_State* L);
		int SetTextColor(lua_State* L);
		int GetTextSize(lua_State* L);
		int OutlinedCircle(lua_State* L);
		int Text(lua_State* L);
		int CreateFont(lua_State* L);
		int SetFont(lua_State* L);
	}

	namespace render
	{
		extern const luaL_Reg renderlib[];

		void luaopen_render(lua_State* L);
		int GetColorModulation(lua_State* L);
		int SetColorModulation(lua_State* L);
		int GetBlend(lua_State* L);
		int SetBlend(lua_State* L);
		int ForcedMaterialOverride(lua_State* L);
		int GetMaterialOverride(lua_State* L);
	}

	namespace materials
	{
		extern const luaL_Reg matslib[];

		void luaopen_materials(lua_State* L);

		int Create(lua_State* L);
	}

	namespace client
	{
		extern const luaL_Reg clientlib[];
		void luaopen_client(lua_State* L);
		int GetNetChannel(lua_State* L);
		int GetConVar(lua_State* L);
		int SetConVar(lua_State* L);
		int ChatSay(lua_State* L);
		int Command(lua_State* L);
	}
}