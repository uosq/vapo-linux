#pragma once

#include "entity.h"
#include "../definitions/types.h"
#include "../defs.h"
#include "../definitions/bspflags.h"
#include "playerresource.h"
#include "../definitions/classdata.h"

#define	FL_ONGROUND (1<<0)
#define FL_DUCKING (1<<1)
#define	FL_WATERJUMP (1<<2)
#define FL_ONTRAIN (1<<3)
#define FL_INRAIN (1<<4)
#define FL_FROZEN (1<<5)
#define FL_ATCONTROLS (1<<6)
#define	FL_CLIENT (1<<7)
#define FL_FAKECLIENT (1<<8)
#define	FL_INWATER (1<<9)
#define PLAYER_FLAG_BITS 32

enum {
	CLASS_UNDEFINED = 0,
	CLASS_SCOUT,
	CLASS_SNIPER,
	CLASS_SOLDIER,
	CLASS_DEMOMAN,
	CLASS_MEDIC,
	CLASS_HEAVYWEAPONS,
	CLASS_PYRO,
	CLASS_SPY,
	CLASS_ENGINEER
};

struct player_info {
	char name[32];
	int user_id;
	char guid[33];
	unsigned long friends_id;
	char friends_name[32];
	bool fakeplayer;
	bool ishltv;
	unsigned long custom_files[4];
	unsigned char files_downloaded;
};

#define	LIFE_ALIVE 0
#define	LIFE_DYING 1
#define	LIFE_DEAD 2
#define LIFE_RESPAWNABLE 3
#define LIFE_DISCARDBODY 4

enum
{
	WL_NotInWater=0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};

class CTFPlayer : public CBaseEntity {
public:
	NETVAR(GetActiveWeapon, "CBaseCombatCharacter->m_hActiveWeapon", EHANDLE)
	NETVAR(GetNextAttack, "CBaseCombatCharacter->m_flNextAttack", float)
	NETVAR(GetTickBase, "CBasePlayer->m_nTickBase", int)
	NETVAR(GetVelocity, "CBasePlayer->m_vecVelocity[0]", Vector)
	NETVAR(GetFlags, "CBasePlayer->m_fFlags", int)
	NETVAR(GetViewModel, "CBasePlayer->m_hViewModel[0]", EHANDLE)
	NETVAR(GetHealth, "CBasePlayer->m_iHealth", int)
	NETVAR(GetOrigin, "CTFPlayer->m_vecOrigin", Vector)
	NETVAR(m_nForceTauntCam, "CTFPlayer->m_nForceTauntCam", bool)
	NETVAR(m_lifeState, "CBasePlayer->m_lifeState", uint8_t) // 1 byte
	NETVAR(m_nPlayerCond, "CTFPlayer->m_nPlayerCond", int)
	NETVAR(m_nPlayerCondEx, "CTFPlayer->m_nPlayerCondEx", int)
	NETVAR(m_nPlayerCondEx2, "CTFPlayer->m_nPlayerCondEx2", int)
	NETVAR(m_nPlayerCondEx3, "CTFPlayer->m_nPlayerCondEx3", int)
	NETVAR(m_nPlayerCondEx4, "CTFPlayer->m_nPlayerCondEx4", int)
	NETVAR(m_condition_bits, "CTFPlayer->_condition_bits", int)

	// !!! Only has 2 fields ([0], [1])
	NETVAR(m_angEyeAngles, "CTFPlayer->m_angEyeAngles[0]", Vector)

	NETVAR(m_vecViewOffset, "CBasePlayer->m_vecViewOffset[0]", Vector)
	NETVAR(m_flMaxspeed, "CBasePlayer->m_flMaxspeed", float)
	NETVAR(m_flStepSize, "CBasePlayer->m_flStepSize", float)
	NETVAR(m_iDefaultFOV, "CBasePlayer->m_iDefaultFOV", int)
	NETVAR(m_hObserverTarget, "CBasePlayer->m_hObserverTarget", EHANDLE)
	NETVAR(m_iObserverMode, "CBasePlayer->m_iObserverMode", int)
	NETVAR(m_iClass, "CTFPlayer->m_iClass", int)
	NETVAR(m_hGroundEntity, "CBasePlayer->m_hGroundEntity", EHANDLE)
	NETVAR(m_nWaterLevel, "CTFPlayer->m_nWaterLevel", int)

	bool IsAlive()
	{
		return m_lifeState() == LIFE_ALIVE;
	}

	bool InCond(ETFCond cond)
	{
		switch ((int)(cond/32))
		{
			case 0: return m_nPlayerCond() & (1 << cond) || m_condition_bits() & (1 << cond);
			case 1: return m_nPlayerCondEx() & (1 << (cond - 32));
			case 2: return m_nPlayerCondEx2() & (1 << (cond - 64));
			case 3: return m_nPlayerCondEx3() & (1 << (cond - 96));
			case 4: return m_nPlayerCondEx4() & (1 << (cond - 128));
			default: return false;
		}

		return false;
	}

	Vector GetCenter()
	{
		return GetAbsOrigin() + ((m_vecMins() + m_vecMaxs()) * 0.5f);
	}

	Vector GetEyePos()
	{
		return GetAbsOrigin() + m_vecViewOffset();
	}

	bool IsGhost()
	{
		return InCond(ETFCond::TF_COND_HALLOWEEN_GHOST_MODE);
	}

	bool IsTaunting()
	{
		return InCond(ETFCond::TF_COND_TAUNTING);
	}

	bool IsUbercharged()
	{
		return InCond(ETFCond::TF_COND_INVULNERABLE);
	}

	std::string GetName()
	{
		player_info_t info{};
		if (!interfaces::Engine->GetPlayerInfo(GetIndex(), &info))
			return "";

		return info.name;
	}

	void UpdateClientSideAnimation()
	{
		using C_BaseAnimating_UpdateClientSideAnimationFn = void(*)(void*);
		static auto orig = (C_BaseAnimating_UpdateClientSideAnimationFn)sigscan_module("client.so", "80 BF D0 0A 00 00 00 75");
		if (!orig)
			return;

		orig((void*)this);
	}

	int GetWaterLevel()
	{
		int level = WL_NotInWater;
		Vector point = {};
		int cont = 0;

		Vector mins = m_vecMins();
		Vector maxs = m_vecMaxs();
		Vector origin = GetAbsOrigin();

		point.x = origin.x + (mins.x + maxs.x) * 0.5f;
		point.y = origin.y + (mins.y + maxs.y) * 0.5f;
		point.z = origin.z + mins.z + 1;

		cont = interfaces::EngineTrace->GetPointContents(point);
		if (cont & MASK_WATER)
		{
			level = WL_Feet;

			point.z = origin.z + (mins.z + maxs.z)*0.5f;
			cont = interfaces::EngineTrace->GetPointContents(point);
			if (cont & MASK_WATER)
			{
				level = WL_Waist;

				point.z = origin.z + m_vecViewOffset().z;
				cont = interfaces::EngineTrace->GetPointContents(point);
				if (cont & MASK_WATER)
					level = WL_Eyes;
			}
		}

		return level;
	}

	float GetEffectiveInvisibilityLevel()
	{
		// xref: taunt_attr_player_invis_percent
		using GetEffectiveInvisibilityLevelFn = float(*)(void* thisptr);
		static auto orig = reinterpret_cast<GetEffectiveInvisibilityLevelFn>(sigscan_module("client.so", "55 48 89 E5 41 56 41 55 4C 8D AF 78 1E 00 00 41 54 49 89 FC 4C 89 EF 53 E8"));

		if (orig == nullptr)
			return -1;

		return orig(this);
	}
};