#include "classes.h"
#include <cstring>
#include <lua5.4/lua.h>
#include <string>
#include "../../sdk/math.h"
#include "../../sdk/classes/player.h"
#include "../../sdk/classes/cbaseobject.h"
#include "../../sdk/classes/ctfrobotdestruction_robot.h"
#include "../../sdk/netvars/netvar.h"
#include "../entitylist/entitylist.h"
#include "../../sdk/handle_utils.h"

namespace LuaClasses
{
	namespace VectorLua
	{
		const luaL_Reg methods[] =
		{
			{"Set", Set},
			{"Normalize", Normalize},
			{"Length", Length},
			{"Length2D", Length2D},
			{"LengthSqr", LengthSqr},
			{"Length2DSqr", Length2DSqr},
			{"Cross", Cross},
			{"Dot", Dot},
			{"DistTo", DistTo},
			{"DistTo2D", DistTo2D},
			{"ToAngle", ToAngle},
			{"Angles", Angles},
			{"ToString", ToString},
			{nullptr, nullptr},
		};

		void luaopen_vector(lua_State* L)
		{
			luaL_newmetatable(L, "Vector3");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, NewIndex);
			lua_setfield(L, -2, "__newindex");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");

			lua_pushcfunction(L, ToString);
			lua_setfield(L, -2, "__tostring");

			lua_pushcfunction(L, Vector3);
			lua_setglobal(L, "Vector3");
		}

		int Vector3(lua_State* L)
		{
			lua_Number x = luaL_optnumber(L, 1, 0.0);
			lua_Number y = luaL_optnumber(L, 2, 0.0);
			lua_Number z = luaL_optnumber(L, 3, 0.0);

			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			new (v) Vector((float)x, (float)y, (float)z);

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return 1;
		}

		int GC(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			v->~Vector();
			return 0;
		}

		Vector* push_vector(lua_State* L, float x, float y, float z)
		{
			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			new (v) Vector(x, y, z);

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return v;
		}

		Vector* push_vector(lua_State* L, Vector vec)
		{
			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			new (v) Vector(vec.x, vec.y, vec.z);

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return v;
		}

		int Index(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			const char* key = luaL_checkstring(L, 2);

			if (strcmp(key, "x") == 0) { lua_pushnumber(L, v->x); return 1; }
			if (strcmp(key, "y") == 0) { lua_pushnumber(L, v->y); return 1; }
			if (strcmp(key, "z") == 0) { lua_pushnumber(L, v->z); return 1; }

			// fallback to methods
			luaL_getmetatable(L, "Vector3");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}

		int NewIndex(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			const char* key = luaL_checkstring(L, 2);
			float value = (float)luaL_checknumber(L, 3);

			if (strcmp(key, "x") == 0) { v->x = value; return 0; }
			if (strcmp(key, "y") == 0) { v->y = value; return 0; }
			if (strcmp(key, "z") == 0) { v->z = value; return 0; }

			return luaL_error(L, "invalid field '%s'", key);
		}

		int ToString(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			std::string str = (std::to_string(v->x) + ", " + std::to_string(v->y) + ", " + std::to_string(v->z));
			lua_pushlstring(L, str.c_str(), str.size());
			return 1;
		}

		int Set(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float x = 0, y = 0, z = 0;

			x = static_cast<float>(luaL_checknumber(L, 2));
			y = static_cast<float>(luaL_checknumber(L, 3));
			z = static_cast<float>(luaL_checknumber(L, 4));

			vec->Set(x, y, z);
			return 0;
		}

		int Normalize(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Normalize();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length2D(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length2D();
			lua_pushnumber(L, length);
			return 1;
		}

		int LengthSqr(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->LengthSqr();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length2DSqr(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length2DSqr();
			lua_pushnumber(L, length);
			return 1;
		}

		int Cross(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector cross = a->Cross(*b);
			push_vector(L, cross);
			return 1;
		}

		int Dot(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dot = a->Dot(*b);
			push_vector(L, dot);
			return 1;
		}

		int DistTo(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dist = a->DistTo(*b);
			push_vector(L, dist);
			return 1;
		}

		int DistTo2D(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dist = a->DistTo2D(*b);
			push_vector(L, dist);
			return 1;
		}

		int ToAngle(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector angle = a->ToAngle();
			push_vector(L, angle);
			return 1;
		}

		int Angles(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));

			Vector forward, right, up;
			Math::AngleVectors(*a, &forward, &right, &up);

			push_vector(L, forward);
			push_vector(L, right);
			push_vector(L, up);
			return 1;
		}
	}
}

namespace LuaClasses
{
	namespace EntityLua
	{
		const luaL_Reg methods[] =
		{
			{"IsValid", IsValid},
			{"GetName", GetName},
			{"GeClassName", GetClassName},
			{"GetAbsOrigin", GetAbsOrigin},
			{"GetAbsAngles", GetAbsAngles},
			{"GetNetvarInt", GetNetvarInt},
			{"GetNetvarFloat", GetNetvarFloat},
			{"GetNetvarVector", GetNetvarVector},
			{"GetNetvarEntity", GetNetvarEntity},
			{"GetHealth", GetHealth},
			{"GetMaxHealth", GetMaxHealth},
			{"GetTeamNumber", GetTeamNumber},
			{"IsWeapon", IsWeapon},
			{"IsDormant", IsDormant},
			{"SetNetvarInt", SetNetvarInt},
			{"SetNetvarFloat", SetNetvarFloat},
			{"SetNetvarVector", SetNetvarVector},
			{"SetNetvarEntity", SetNetvarEntity},
			{"GetPlayerResources", GetPlayerResources},
			{nullptr, nullptr}
		};

		void luaopen_entity(lua_State* L)
		{
			luaL_newmetatable(L, "Entity");

			// make lua check our methods
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, GC);
    			lua_setfield(L, -2, "__gc");

			lua_pushcfunction(L, EQ);
    			lua_setfield(L, -2, "__eq");
		}

		LuaEntity* push_entity(lua_State* L, CBaseEntity* entity)
		{
			if (!entity)
			{
				lua_pushnil(L);
				return nullptr;
			}

			LuaEntity* le = (LuaEntity*)lua_newuserdata(L, sizeof(LuaEntity));
			le->ent = entity;

			luaL_getmetatable(L, "Entity");
			lua_setmetatable(L, -2);

			return le;
		}

		LuaEntity* CheckEntity(lua_State* L, int idx)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, idx, "Entity"));

			if (le->ent == nullptr)
				luaL_error(L, "Entity is invalid");

			return le;
		}

		int IsValid(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			lua_pushboolean(L, le->ent != nullptr);
			return 1;
		}

		int GC(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			le->ent = nullptr; // just clear reference
			return 0;
		}

		int GetName(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsPlayer())
			{
				CTFPlayer* player = reinterpret_cast<CTFPlayer*>(le->ent);
				std::string name = player->GetName();
				lua_pushlstring(L, name.c_str(), name.size());
				return 1;
			}
			else
			{
				const char* name = le->ent->GetClientClass()->networkName;
				lua_pushstring(L, "");
			}

			return 1;
		}

		int GetClassName(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			const char* name = le->ent->GetClientClass()->networkName;
			lua_pushstring(L, name);

			return 1;
		}

		int GetAbsOrigin(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent != nullptr)
			{
				Vector origin = le->ent->GetAbsOrigin();
				LuaClasses::VectorLua::push_vector(L, origin);
			}
			else
			{
				lua_pushnil(L);
			}

			return 1;
		}

		int GetAbsAngles(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent != nullptr)
			{
				Vector angles = le->ent->GetAbsAngles();
				LuaClasses::VectorLua::push_vector(L, angles);
			}
			else
			{
				lua_pushnil(L);
			}

			return 1;
		}

		// entity:GetNetvarInt("CBasePlayer->m_iHealth")
		int GetNetvarInt(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* str = luaL_checkstring(L, 2);
			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				int value = *reinterpret_cast<int*>(entityPtr + offset);
				lua_pushnumber(L, value);
			}
			else
			{
				lua_pushnil(L);
			}
			return 1;
		}

		int GetNetvarFloat(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* str = luaL_checkstring(L, 2);
			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				float value = *reinterpret_cast<float*>(entityPtr + offset);
				lua_pushnumber(L, value);
			}
			else
			{
				lua_pushnil(L);
			}
			return 1;
		}

		int GetNetvarVector(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* str = luaL_checkstring(L, 2);
			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				Vector value = *reinterpret_cast<Vector*>(entityPtr + offset);
				LuaClasses::VectorLua::push_vector(L, value);
			}
			else
			{
				lua_pushnil(L);
			}
			return 1;
		}

		int GetNetvarEntity(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* str = luaL_checkstring(L, 2);
			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				auto handle = *reinterpret_cast<EHANDLE*>(entityPtr + offset);
				CBaseEntity* ent = HandleAs<CBaseEntity*>(handle);
				push_entity(L, ent);
			}
			else
			{
				lua_pushnil(L);
			}

			return 1;
		}

		int GetHealth(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsPlayer())
			{
				CTFPlayer* player = static_cast<CTFPlayer*>(le->ent);
				lua_pushinteger(L, player->GetHealth());
				return 1;
			}

			if (le->ent->IsBuilding())
			{
				CBaseObject* obj = reinterpret_cast<CBaseObject*>(le->ent);
				lua_pushinteger(L, obj->m_iMaxHealth());
				return 1;
			}

			if (le->ent->IsRobot())
			{
				CTFRobotDestruction_Robot* robot = static_cast<CTFRobotDestruction_Robot*>(le->ent);
				lua_pushinteger(L, robot->m_iHealth());
				return 1;
			}

			// unknown
			// or I don't know how to get the health
			lua_pushinteger(L, -1);
			return 1;
		}

		int GetMaxHealth(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsPlayer())
			{
				if (EntityList::m_pPlayerResource != nullptr)
				{
					lua_pushinteger(L, EntityList::m_pPlayerResource->m_iMaxHealth(le->ent->GetIndex()));
					return 1;
				}
			}

			if (le->ent->IsBuilding())
			{
				CBaseObject* obj = reinterpret_cast<CBaseObject*>(le->ent);
				lua_pushinteger(L, obj->m_iMaxHealth());
				return 1;
			}

			if (le->ent->IsRobot())
			{
				CTFRobotDestruction_Robot* robot = static_cast<CTFRobotDestruction_Robot*>(le->ent);
				lua_pushinteger(L, robot->m_iMaxHealth());
				return 1;
			}

			lua_pushinteger(L, -1);
			return 1;
		}

		int EQ(lua_State *L)
		{
			LuaEntity* self = CheckEntity(L, 1);
			LuaEntity* other = CheckEntity(L, 2);
			if (self && other)
			{
				lua_pushboolean(L, self->ent->GetIndex() == other->ent->GetIndex());
				return 1;
			}

			lua_pushboolean(L, false);
			return 1;
		}

		int GetMins(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le == nullptr || le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsBuilding())
			{
				CBaseObject* obj = reinterpret_cast<CBaseObject*>(le->ent);
				LuaClasses::VectorLua::push_vector(L, obj->m_vecBuildMins());
				return 1;
			}

			LuaClasses::VectorLua::push_vector(L, le->ent->m_vecMins());
			return 1;
		}

		int GetMaxs(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsBuilding())
			{
				CBaseObject* obj = reinterpret_cast<CBaseObject*>(le->ent);
				LuaClasses::VectorLua::push_vector(L, obj->m_vecBuildMaxs());
				return 1;
			}

			LuaClasses::VectorLua::push_vector(L, le->ent->m_vecMaxs());
			return 1;
		}

		int GetTeamNumber(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, le->ent->m_iTeamNum());
			return 1;
		}

		int IsWeapon(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushboolean(L, le->ent->IsWeapon());
			return 1;
		}

		int IsDormant(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushboolean(L, le->ent->IsDormant());
			return 1;
		}

		int IsAlive(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (le->ent->IsPlayer())
			{
				CTFPlayer* player = static_cast<CTFPlayer*>(le->ent);
				lua_pushboolean(L, player->IsAlive());
				return 1;
			}

			if (le->ent->IsBuilding())
			{
				CBaseObject* obj = reinterpret_cast<CBaseObject*>(le->ent);
				lua_pushboolean(L, obj->m_iHealth() > 0);
				return 1;
			}

			lua_pushboolean(L, false);
			return 1;
		}

		// entity:SetNetvarInt("CBasePlayer->m_iHealth", 10)
		int SetNetvarInt(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
				return 0;

			if (lua_isnoneornil(L, 2) || lua_isnoneornil(L, 3))
				return 0;

			const char* str = luaL_checkstring(L, 2);
			int number = luaL_checkinteger(L, 3);

			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				int* value = reinterpret_cast<int*>(entityPtr + offset);
				*value = number;
			}

			return 0;
		}

		int SetNetvarFloat(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
				return 0;

			if (lua_isnoneornil(L, 2) || lua_isnoneornil(L, 3))
				return 0;

			const char* str = luaL_checkstring(L, 2);
			float number = luaL_checknumber(L, 3);

			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				float* value = reinterpret_cast<float*>(entityPtr + offset);
				*value = number;
			}

			return 0;
		}

		int SetNetvarVector(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
				return 0;

			if (lua_isnoneornil(L, 2) || lua_isnoneornil(L, 3))
				return 0;

			const char* str = luaL_checkstring(L, 2);
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 3, "Vector3"));

			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				Vector* value = reinterpret_cast<Vector*>(entityPtr + offset);
				*value = *vec;
			}

			return 0;
		}

		// this will likely crash the game
		// idk why someone would want to do this anyway
		int SetNetvarEntity(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
				return 0;

			if (lua_isnoneornil(L, 2) || lua_isnoneornil(L, 3))
				return 0;

			LuaEntity* le2 = CheckEntity(L, 3);
			if (le2->ent == nullptr)
				return 0;

			const char* str = luaL_checkstring(L, 2);

			auto it = netvars.find(fnv::Hash(str));
			if (it != netvars.end())
			{
				auto& offset = it->second;
				uintptr_t entityPtr = reinterpret_cast<uintptr_t>(le->ent);
				EHANDLE* value = reinterpret_cast<EHANDLE*>(entityPtr + offset);
				CBaseEntity* target = HandleAs<CBaseEntity*>(*value);
				*target = *le2->ent;
			}

			return 0;
		}

		int GetPlayerResources(lua_State* L)
		{
			CTFPlayerResource* resource = EntityList::m_pPlayerResource;
			if (resource == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_newtable(L);

			int index = le->ent->GetIndex();
			
			lua_pushinteger(L, resource->m_iTotalScore(index));
			lua_setfield(L, -2, "m_iTotalScore");
			
			lua_pushinteger(L, resource->m_iMaxHealth(index));
			lua_setfield(L, -2, "m_iMaxHealth");
			
			lua_pushinteger(L, resource->m_iMaxBuffedHealth(index));
			lua_setfield(L, -2, "m_iMaxBuffedHealth");
			
			lua_pushinteger(L, resource->m_iPlayerClass(index));
			lua_setfield(L, -2, "m_iPlayerClass");
			
			lua_pushinteger(L, resource->m_bArenaSpectator(index));
			lua_setfield(L, -2, "m_bArenaSpectator");
			
			lua_pushinteger(L, resource->m_iActiveDominations(index));
			lua_setfield(L, -2, "m_iActiveDominations");
			
			lua_pushnumber(L, resource->m_flNextRespawnTime(index));
			lua_setfield(L, -2, "m_flNextRespawnTime");
			
			lua_pushinteger(L, resource->m_iChargeLevel(index));
			lua_setfield(L, -2, "m_iChargeLevel");
			
			lua_pushinteger(L, resource->m_iDamage(index));
			lua_setfield(L, -2, "m_iDamage");
			
			lua_pushinteger(L, resource->m_iDamageAssist(index));
			lua_setfield(L, -2, "m_iDamageAssist");
			
			lua_pushinteger(L, resource->m_iDamageBoss(index));
			lua_setfield(L, -2, "m_iDamageBoss");
			
			lua_pushinteger(L, resource->m_iHealing(index));
			lua_setfield(L, -2, "m_iHealing");
			
			lua_pushinteger(L, resource->m_iHealingAssist(index));
			lua_setfield(L, -2, "m_iHealingAssist");
			
			lua_pushinteger(L, resource->m_iDamageBlocked(index));
			lua_setfield(L, -2, "m_iDamageBlocked");
			
			lua_pushinteger(L, resource->m_iCurrencyCollected(index));
			lua_setfield(L, -2, "m_iCurrencyCollected");
			
			lua_pushinteger(L, resource->m_iBonusPoints(index));
			lua_setfield(L, -2, "m_iBonusPoints");
			
			lua_pushinteger(L, resource->m_iPlayerLevel(index));
			lua_setfield(L, -2, "m_iPlayerLevel");
			
			lua_pushinteger(L, resource->m_iStreaks(index));
			lua_setfield(L, -2, "m_iStreaks");
			
			lua_pushinteger(L, resource->m_iUpgradeRefundCredits(index));
			lua_setfield(L, -2, "m_iUpgradeRefundCredits");
			
			lua_pushinteger(L, resource->m_iBuybackCredits(index));
			lua_setfield(L, -2, "m_iBuybackCredits");
			
			lua_pushinteger(L, resource->m_iPartyLeaderRedTeamIndex(index));
			lua_setfield(L, -2, "m_iPartyLeaderRedTeamIndex");
			
			lua_pushinteger(L, resource->m_iPartyLeaderBlueTeamIndex(index));
			lua_setfield(L, -2, "m_iPartyLeaderBlueTeamIndex");
			
			lua_pushinteger(L, resource->m_iEventTeamStatus(index));
			lua_setfield(L, -2, "m_iEventTeamStatus");
			
			lua_pushinteger(L, resource->m_iPlayerClassWhenKilled(index));
			lua_setfield(L, -2, "m_iPlayerClassWhenKilled");
			
			lua_pushinteger(L, resource->m_iConnectionState(index));
			lua_setfield(L, -2, "m_iConnectionState");
			
			lua_pushnumber(L, resource->m_flConnectTime(index));
			lua_setfield(L, -2, "m_flConnectTime");

			return 1;
		}
	}
}