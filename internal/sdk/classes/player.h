#pragma once

#include "entity.h"
#include "../definitions/types.h"
#include "../defs.h"

struct user_cmd;

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

	bool IsAlive()
	{
		//interfaces::vstdlib->ConsolePrintf("%d\n", m_lifeState());
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
};