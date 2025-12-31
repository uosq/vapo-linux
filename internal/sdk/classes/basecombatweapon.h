#pragma once

#include "../../vtables.h"
#include "../netvars/netvar.h"
#include "econentity.h"

class CBaseCombatWeapon : public CEconEntity
{
public:
	NETVAR(m_iClip1, "CBaseCombatWeapon->m_iClip1", int)
	NETVAR(m_iClip2, "CBaseCombatWeapon->m_iClip2", int)
	NETVAR(m_iPrimaryAmmoType, "CBaseCombatWeapon->m_iPrimaryAmmoType", int)
	NETVAR(m_iSecondaryAmmoType, "CBaseCombatWeapon->m_iSecondaryAmmoType", int)
	NETVAR(m_nViewModelIndex, "CBaseCombatWeapon->m_nViewModelIndex", int)
	NETVAR(m_bFlipViewModel, "CBaseCombatWeapon->m_bFlipViewModel", bool)
	NETVAR(m_flNextPrimaryAttack, "CBaseCombatWeapon->m_flNextPrimaryAttack", float)
	NETVAR(m_flNextSecondaryAttack, "CBaseCombatWeapon->m_flNextSecondaryAttack", float)
	NETVAR(m_nNextThinkTick, "CBaseCombatWeapon->m_nNextThinkTick", int)
	NETVAR(m_flTimeWeaponIdle, "CBaseCombatWeapon->m_flTimeWeaponIdle", float)
	NETVAR(m_iViewModelIndex, "CBaseCombatWeapon->m_iViewModelIndex", int)
	NETVAR(m_iWorldModelIndex, "CBaseCombatWeapon->m_iWorldModelIndex", int)
	NETVAR(m_iState, "CBaseCombatWeapon->m_iState", int)
	NETVAR(m_hOwner, "CBaseCombatWeapon->m_hOwner", EHANDLE)

	NETVAR_OFFSET(m_bInReload, "CBaseCombatWeapon->m_flTimeWeaponIdle", bool, 4)
	NETVAR_OFFSET(m_bRemoveable, "CBaseCombatWeapon->m_iState", bool, -12)
	NETVAR_OFFSET(m_bReloadsSingly, "CBaseCombatWeapon->m_iClip2", bool, 24)

	int GetMaxClip1() { return vtable::call<322 + 1, int>(this); }
	bool CanBeSelected() { return vtable::call<233 + 1, bool>(this); }
	void CheckReload() { return vtable::call<278 + 1, void>(this); }
	const char* GetName() { return vtable::call<334 + 1, const char*>(this); }
};