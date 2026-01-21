#pragma once

#include "../../sdk/interfaces/interfaces.h"

#include "pluto/lua.h"
#include "pluto/lua.hpp"
#include "pluto/lualib.h"

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
		int GetNetChannel(lua_State* L);

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
		int DrawScreenSpaceRectangle(lua_State* L);
		int DrawScreenSpaceQuad(lua_State* L);
		int GetViewport(lua_State* L);
		int DepthRange(lua_State* L);
		int GetDepthRange(lua_State* L);
		int SetRenderTarget(lua_State* L);
		int GetRenderTarget(lua_State* L);
		int ClearBuffers(lua_State* L);
		int ClearColor3ub(lua_State* L);
		int ClearColor4ub(lua_State* L);
		int OverrideDepthEnable(lua_State* L);
		int OverrideAlphaEnable(lua_State* L);
		int OverrideColorEnable(lua_State* L);
		int PushRenderTargetAndViewport(lua_State* L);
		int PopRenderTargetAndViewport(lua_State* L);
		int SetStencilEnable(lua_State* L);
		int SetStencilFailOperation(lua_State* L);
		int SetStencilZFailOperation(lua_State* L);
		int SetStencilPassOperation(lua_State* L);
		int SetStencilCompareFunction(lua_State* L);
		int SetStencilReferenceValue(lua_State* L);
		int SetStencilTestMask(lua_State* L);
		int SetStencilWriteMask(lua_State* L);
		int ClearStencilBufferRectangle(lua_State* L);
		int Viewport(lua_State* L);
	}

	namespace materials
	{
		extern const luaL_Reg matslib[];

		void luaopen_materials(lua_State* L);

		int Create(lua_State* L);
		int FindMaterial(lua_State* L);
		int FindTexture(lua_State* L);
		int CreateTextureRenderTarget(lua_State* L);
		int Enumerate(lua_State* L);
	}

	namespace client
	{
		extern const luaL_Reg clientlib[];
		void luaopen_client(lua_State* L);
		int GetConVar(lua_State* L);
		int SetConVar(lua_State* L);
		int ChatSay(lua_State* L);
		int Command(lua_State* L);
	}

	namespace clientstate
	{
		extern const luaL_Reg clientlib[];
		void luaopen_clientstate(lua_State* L);

		int GetChokedCommands(lua_State* L);
		int GetLastOutgoingCommand(lua_State* L);
		int GetClientSignonState(lua_State* L);
		int GetDeltaTick(lua_State* L);
		int GetLastCommandAck(lua_State* L);
		int ForceFullUpdate(lua_State* L);
	}

	namespace input
	{
		extern const luaL_Reg inputlib[];
		void luaopen_input(lua_State* L);
		int IsButtonDown(lua_State* L);
		int IsButtonPressed(lua_State* L);
		int IsButtonReleased(lua_State* L);
		int SetCursorAlwaysVisible(lua_State* L);
		int IsCursorVisible(lua_State* L);
		int GetPollTick(lua_State* L);
	}
}