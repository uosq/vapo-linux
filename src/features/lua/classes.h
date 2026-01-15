#pragma once

#include "../../sdk/definitions/types.h"
#include "../../sdk/classes/entity.h"

extern "C"
{
	#include <lua5.4/lua.h>
	#include <lua5.4/lua.hpp>
	#include <lua5.4/lualib.h>
}

struct LuaEntity
{
    CBaseEntity* ent;
};

namespace LuaClasses
{
	namespace VectorLua
	{
		extern const luaL_Reg methods[];
		void luaopen_vector(lua_State* L);
		Vector* push_vector(lua_State* L, float x = 0, float y = 0, float z = 0);
		Vector* push_vector(lua_State* L, Vector vec);
		int Vector3(lua_State* L);
		
		int GC(lua_State* L);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);
		int ToString(lua_State* L);

		int Init(lua_State* L);
		int Set(lua_State* L);
		int Normalize(lua_State* L);
		int Length(lua_State* L);
		int Length2D(lua_State* L);
		int LengthSqr(lua_State* L);
		int Length2DSqr(lua_State* L);
		int Cross(lua_State* L);
		int Dot(lua_State* L);
		int DistTo(lua_State* L);
		int DistTo2D(lua_State* L);
		int ToAngle(lua_State* L);
		int Angles(lua_State* L);
	};

	namespace EntityLua
	{
		extern const luaL_Reg methods[];
		void luaopen_entity(lua_State* L);
		LuaEntity* push_entity(lua_State* L, CBaseEntity* entity);
		LuaEntity* CheckEntity(lua_State* L, int idx);

		int GC(lua_State* L);
		int EQ(lua_State* L);

		int IsValid(lua_State* L);

		int GetName(lua_State* L);
		int GetClassName(lua_State* L);

		int GetAbsOrigin(lua_State* L);
		int GetAbsAngles(lua_State* L);

		int GetNetvarInt(lua_State* L);
		int GetNetvarFloat(lua_State* L);
		int GetNetvarVector(lua_State* L);
		int GetNetvarEntity(lua_State* L);

		int SetNetvarInt(lua_State* L);
		int SetNetvarFloat(lua_State* L);
		int SetNetvarVector(lua_State* L);
		int SetNetvarEntity(lua_State* L);

		int GetHealth(lua_State* L);
		int GetMaxHealth(lua_State* L);

		int GetMins(lua_State* L);
		int GetMaxs(lua_State* L);

		int GetTeamNumber(lua_State* L);

		int IsWeapon(lua_State* L);
		int IsDormant(lua_State* L);
		int IsAlive(lua_State* L);

		int GetPlayerResources(lua_State* L);
	};
}