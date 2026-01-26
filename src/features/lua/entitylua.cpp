#include "classes.h"

#include "../../sdk/classes/player.h"
#include "../../sdk/classes/cbaseobject.h"
#include "../../sdk/classes/ctfrobotdestruction_robot.h"
#include "../../sdk/netvars/netvar.h"
#include "../entitylist/entitylist.h"
#include "../../sdk/handle_utils.h"
#include "pluto/lua.h"
#include "../prediction/prediction.h"

namespace LuaClasses
{
	namespace EntityLua
	{
		const luaL_Reg methods[] =
		{
			{"IsValid", IsValid},
			{"GetName", GetName},
			{"GetClassName", GetClassName},
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
			{"IsAlive", IsAlive},
			{"IsPlayer", IsPlayer},
			{"GetWeaponID", GetWeaponID},
			{"GetClassID", GetClassID},
			{"GetFirstMoveChild", GetFirstMoveChild},
			{"GetNextMovePeer", GetNextMovePeer},
			{"AttributeHookInt", AttributeHookValueInt},
			{"AttributeHookFloat", AttributeHookValueFloat},
			{"GetWeaponType", GetWeaponType},
			{"IsInReload", IsInReload},
			{"GetSmackTime", GetSmackTime},
			{"GetIndex", GetIndex},
			{"Predict", Predict},
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
				lua_pushinteger(L, value);
				return 1;
			}

			lua_pushnil(L);
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
				return 1;
			}

			lua_pushnil(L);
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
				return 1;
			}

			lua_pushnil(L);
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
				return 1;
			}

			lua_pushnil(L);
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

			if (self->ent && other->ent)
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
			if (le->ent == nullptr)
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
			if (le->ent == nullptr || !le->ent->IsPlayer())
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
			
			lua_pushboolean(L, resource->m_bArenaSpectator(index));
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

		int IsPlayer(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushboolean(L, le->ent->IsPlayer());
			return 1;
		}

		int GetWeaponID(lua_State *L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (!le->ent->IsWeapon())
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<CTFWeaponBase*>(le->ent)->GetWeaponID());
			return 1;
		}

		int GetClassID(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, static_cast<int>(le->ent->GetClassID()));
			return 1;
		}

		int GetFirstMoveChild(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			auto moveChild = le->ent->FirstShadowChild();
			if (moveChild == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			push_entity(L, static_cast<CBaseEntity*>(moveChild));
			return 1;
		}

		int GetNextMovePeer(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			auto movePeer = le->ent->NextShadowPeer();
			if (movePeer == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			push_entity(L, static_cast<CBaseEntity*>(movePeer));
			return 1;
		}

		int AttributeHookValueInt(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* attrib = luaL_checkstring(L, 2);
			int number = luaL_optinteger(L, 3, 1);
			int value = static_cast<int>(AttributeHookValue(static_cast<float>(number), attrib, le->ent, nullptr, true));
			lua_pushinteger(L, value);
			return 1;
		}

		int AttributeHookValueFloat(lua_State* L)
		{
			LuaEntity* le = CheckEntity(L, 1);
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			const char* attrib = luaL_checkstring(L, 2);
			int number = luaL_optnumber(L, 3, 1.0f);
			float value = AttributeHookValue(number, attrib, le->ent, nullptr, true);
			lua_pushnumber(L, value);
			return 1;
		}

		int GetWeaponType(lua_State* L)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
				return 0;

			if (!le->ent->IsWeapon())
				return 0;

			// why c++ syntax is so fucking ugly?
			lua_pushinteger(L, static_cast<int>(static_cast<CTFWeaponBase*>(le->ent)->GetWeaponType()));
			return 1;
		}

		int IsInReload(lua_State* L)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
				return 0;

			if (!le->ent->IsWeapon())
				return 0;

			lua_pushboolean(L, static_cast<CTFWeaponBase*>(le->ent)->m_bInReload());
			return 1;
		}

		int GetSmackTime(lua_State* L)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
				return 0;

			if (!le->ent->IsWeapon())
				return 0;

			CTFWeaponBase* pWeapon = static_cast<CTFWeaponBase*>(le->ent);
			if (!pWeapon || !pWeapon->IsMelee())
				return 0;

			lua_pushnumber(L, pWeapon->m_flSmackTime());
			return 1;
		}

		int GetIndex(lua_State* L)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
				return 0;

			lua_pushinteger(L, le->ent->entindex());
			return 1;
		}

		// entity:Predict
		int Predict(lua_State* L)
		{
			LuaEntity* le = static_cast<LuaEntity*>(luaL_checkudata(L, 1, "Entity"));
			if (le->ent == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			if (!le->ent->IsPlayer())
			{
				lua_pushnil(L);
				return 1;
			}

			float seconds = luaL_checknumber(L, 2);

			std::vector<Vector> path;
			PlayerPrediction::Predict(static_cast<CTFPlayer*>(le->ent), seconds, path);

			lua_newtable(L);

			int i = 1;
			for (const Vector& v : path)
			{
				LuaClasses::VectorLua::push_vector(L, v);
				lua_rawseti(L, -2, i++);
			}

			return 1;
		}
	}
}