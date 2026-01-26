#pragma once

#include "../sdk/interfaces/interfaces.h"
#include "../sdk/classes/entity.h"
#include "../sdk/classes/player.h"
#include "../sdk/helpers/helper.h"
#include "../sdk/definitions/ivmodelrender.h"
#include "../sdk/definitions/keyvalues.h"

#include "../features/chams/chams.h"
#include "../features/glow/glow.h"

#include "../features/lua/hooks.h"
#include "../features/lua/api.h"
#include "../features/lua/classes.h"

struct DrawModelContext
{
	IVModelRender* thisptr;
	DrawModelState_t state;
	ModelRenderInfo_t pInfo;
	matrix3x4* pCustomBoneToWorld;
};

static DrawModelContext ctx;
static int LuaCallDME(lua_State* L);

DECLARE_VTABLE_HOOK(DrawModelExecute, void, (IVModelRender* thisptr, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld))
{
	//if (settings.esp.chams && !Chams::m_bRunning && Chams::ShouldHide(pInfo.entity_index))
		//return;

	if (interfaces::Engine->IsTakingScreenshot())
		return originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);

	if (!Chams::m_bRunning && !Glow::m_bRunning)
	{
		float color[3] = {1, 1, 1};
		interfaces::RenderView->SetColorModulation(color);
		interfaces::RenderView->SetBlend(1.0f);
		interfaces::ModelRender->ForcedMaterialOverride(nullptr);
	}

	if (LuaHookManager::HasHooks("DrawModel"))
	{
		lua_newtable(Lua::m_luaState);

		CBaseEntity* ent = static_cast<CBaseEntity*>(interfaces::EntityList->GetClientEntity(pInfo.entity_index));

		if (ent != nullptr)
			LuaClasses::EntityLua::push_entity(Lua::m_luaState, ent);
		else
			lua_pushnil(Lua::m_luaState);

		lua_setfield(Lua::m_luaState, -2, "entity");

		lua_pushcfunction(Lua::m_luaState, LuaCallDME);
		lua_setfield(Lua::m_luaState, -2, "Call");

		lua_pushboolean(Lua::m_luaState, Chams::m_bRunning);
		lua_setfield(Lua::m_luaState, - 2, "is_chams");

		lua_pushboolean(Lua::m_luaState, Glow::m_bRunning);
		lua_setfield(Lua::m_luaState, - 2, "is_glow");

		ctx.state = state;
		ctx.pCustomBoneToWorld = pCustomBoneToWorld;
		ctx.pInfo = pInfo;
		ctx.thisptr = thisptr;

		LuaHookManager::Call(Lua::m_luaState, "DrawModel", 1);
	}

	if (Chams::m_bRunning || Glow::m_bRunning)
		return originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);

	if (Chams::ShouldHide(pInfo.entity_index) || Glow::ShouldHide(pInfo.entity_index))
		return;

	originalDrawModelExecute(thisptr, state, pInfo, pCustomBoneToWorld);
}

static int LuaCallDME(lua_State* L)
{
	originalDrawModelExecute(ctx.thisptr, ctx.state, ctx.pInfo, ctx.pCustomBoneToWorld);
	return 0;
}

inline void HookDrawModelExecute(void)
{
	INSTALL_VTABLE_HOOK(DrawModelExecute, interfaces::ModelRender, 19);

	constexpr Color_t color = {100, 255, 100, 255};
	helper::console::ColoredPrint("IModelRender::DrawModelExecute hooked\n", color);
}