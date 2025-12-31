#pragma once

#include "../netvars/netvar.h"
#include "../definitions/cbasehandle.h"
#include "../definitions/cbaseflex.h"

class C_BaseCombatWeapon;
class C_WeaponCombatShield;

#define BCC_DEFAULT_LOOK_TOWARDS_TOLERANCE 0.9f

class CBaseCombatCharacter : public CBaseFlex
{
	NETVAR(m_flNextAttack, "CBaseCombatCharacter->m_flNextAttack", float)
	NETVAR(m_hActiveWeapon, "CBaseCombatCharacter->m_hActiveWeapon", EHANDLE)
};