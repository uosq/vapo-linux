#pragma once

#include "../definitions/weaponinfo.h"
#include "basecombatweapon.h"
#include "entity.h"
#include "basecombatcharacter.h"
#include "../defs.h"
#include "../handle_utils.h"
#include "player.h"

class CTFPlayer;
class CTFGrenadePipebombProjectile;

// source: https://github.com/rei-2/Amalgam/blob/bffae9999cf35a5fbdeb92387b9fae58796b8939/Amalgam/src/SDK/Definitions/Main/CTFWeaponBase.h
// yes, i am pasting a lot of amalgam
// do i care? no, as im not gonna write all ths shit from scratch

typedef unsigned short WEAPON_FILE_INFO_HANDLE;

// xref: CHudCrosshair
// Almost all of the functions with 1 parameter is GetTFWpnData
// Then to get the ofset for the handle you do
// go inside GetTFWpnData
// it will call GetFileWeaponInfoFromHandle
// whatever it passes (this + 0xsomething), the 0x123 is the offset
inline FileWeaponInfo_t* Rebuild_GetFileWeaponInfoFromHandle(void* handle)
{
	using GetFileWeaponInfoFromHandleFn = FileWeaponInfo_t*(*)(void*);
	static auto orig = (GetFileWeaponInfoFromHandleFn)sigscan_module("client.so", "66 3B 3D ? ? ? ? 48 8D 05");
	return orig(handle);
}

class CTFWeaponBase : public CBaseCombatWeapon
{
public:
	NETVAR(m_bLowered, "CTFWeaponBase->m_bLowered", bool)
	NETVAR(m_iReloadMode, "CTFWeaponBase->m_iReloadMode", int)
	NETVAR(m_bResetParity, "CTFWeaponBase->m_bResetParity", bool)
	NETVAR(m_bReloadedThroughAnimEvent, "CTFWeaponBase->m_bReloadedThroughAnimEvent", bool)
	NETVAR(m_bDisguiseWeapon, "CTFWeaponBase->m_bDisguiseWeapon", bool)
	NETVAR(m_flLastCritCheckTime, "CTFWeaponBase->m_flLastCritCheckTime", float)
	NETVAR(m_flReloadPriorNextFire, "CTFWeaponBase->m_flReloadPriorNextFire", float)
	NETVAR(m_flLastFireTime, "CTFWeaponBase->m_flLastFireTime", float)
	NETVAR(m_flEffectBarRegenTime, "CTFWeaponBase->m_flEffectBarRegenTime", float)
	NETVAR(m_flObservedCritChance, "CTFWeaponBase->m_flObservedCritChance", float)
	NETVAR(m_flEnergy, "CTFWeaponBase->m_flEnergy", float)
	NETVAR(m_hExtraWearable, "CTFWeaponBase->m_hExtraWearable", EHANDLE)
	NETVAR(m_hExtraWearableViewModel, "CTFWeaponBase->m_hExtraWearableViewModel", EHANDLE)
	NETVAR(m_bBeingRepurposedForTaunt, "CTFWeaponBase->m_bBeingRepurposedForTaunt", bool)
	NETVAR(m_nKillComboClass, "CTFWeaponBase->m_nKillComboClass", int)
	NETVAR(m_nKillComboCount, "CTFWeaponBase->m_nKillComboCount", int)
	NETVAR(m_flInspectAnimEndTime, "CTFWeaponBase->m_flInspectAnimEndTime", float)
	NETVAR(m_nInspectStage, "CTFWeaponBase->m_nInspectStage", int)
	NETVAR(m_iConsecutiveShots, "CTFWeaponBase->m_iConsecutiveShots", int)

	// gotta check, but these are probably all useless or return garbage dataa
	NETVAR_OFFSET(m_flSmackTime, "CTFWeaponBase->m_nInspectStage", float, 28);
	NETVAR_OFFSET(m_flCritTokenBucket, "CTFWeaponBase->m_iReloadMode", float, -244);
	NETVAR_OFFSET(m_nCritChecks, "CTFWeaponBase->m_iReloadMode", int, -240);
	NETVAR_OFFSET(m_nCritSeedRequests, "CTFWeaponBase->m_iReloadMode", int, -236);
	NETVAR_OFFSET(m_iWeaponMode, "CTFWeaponBase->m_iReloadMode", int, -4);
	NETVAR_OFFSET(m_flCritTime, "CTFWeaponBase->m_flLastCritCheckTime", float, -4);
	NETVAR_OFFSET(m_iCurrentSeed, "CTFWeaponBase->m_flLastCritCheckTime", int, 8);
	NETVAR_OFFSET(m_flLastRapidFireCritCheckTime, "CTFWeaponBase->m_flLastCritCheckTime", float, 12);

	// dentro de CTFWeaponBase!
	const FileWeaponInfo_t* GetWeaponInfo()
	{
		// offset from EDI,word ptr [RDI + 0xF12] in CCombatWeapon(?)->GetTFWpnData()
		uintptr_t handleAddress = reinterpret_cast<uintptr_t>(this) + 0xf12;
		void* handleValue = *(void**)handleAddress;
		return Rebuild_GetFileWeaponInfoFromHandle(handleValue);
	}

	int GetWeaponID() {
		// xref: -use_action_slot_item_server
		// func: EndUseActionSlotItem
		/*
			we want the 0xe18 offset
  			iVar3 = (**(code **)(*plVar7 + 0xe18))(plVar7);
  			if (iVar3 != 0x65) goto LAB_01dc047d;
  			uVar5 = FUN_01fdca00(0x40);
  			FUN_01fda880(uVar5,"-use_action_slot_item_server");
		*/
		using Fn = int(*)(void*);
		auto vtable = *reinterpret_cast<void***>(this);
		auto fn = reinterpret_cast<Fn>(vtable[0xE18 / sizeof(void*)]);
		return fn(this);
	}

	bool IsInReload() { return m_bInReload() || m_iReloadMode() != 0; }
	int GetSlot() { return GetWeaponInfo()->iSlot; }

	EWeaponType GetWeaponType()
	{
		if (GetSlot() == EWeaponSlot::SLOT_MELEE || GetWeaponID() == TF_WEAPON_BUILDER)
			return EWeaponType::MELEE;

		switch (m_iItemDefinitionIndex())
		{
			case Soldier_s_TheBuffBanner:
			case Soldier_s_FestiveBuffBanner:
			case Soldier_s_TheBattalionsBackup:
			case Soldier_s_TheConcheror:
			case Scout_s_BonkAtomicPunch:
			case Scout_s_CritaCola:
				return EWeaponType::UNKNOWN;
		}

		switch (GetWeaponID())
		{
			case TF_WEAPON_PDA:
			case TF_WEAPON_PDA_ENGINEER_BUILD:
			case TF_WEAPON_PDA_ENGINEER_DESTROY:
			case TF_WEAPON_PDA_SPY:
			case TF_WEAPON_PDA_SPY_BUILD:
			case TF_WEAPON_INVIS:
			case TF_WEAPON_BUFF_ITEM:
			case TF_WEAPON_GRAPPLINGHOOK:
			case TF_WEAPON_ROCKETPACK:
				return EWeaponType::UNKNOWN;

			case TF_WEAPON_CLEAVER:
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_RAYGUN:
			case TF_WEAPON_FLAMETHROWER:
			case TF_WEAPON_FLAME_BALL:
			case TF_WEAPON_FLAREGUN:
			case TF_WEAPON_FLAREGUN_REVENGE:
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_CANNON:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
			case TF_WEAPON_DRG_POMSON:
			case TF_WEAPON_CROSSBOW:
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			case TF_WEAPON_COMPOUND_BOW:
			case TF_WEAPON_JAR:
			case TF_WEAPON_JAR_MILK:
			case TF_WEAPON_JAR_GAS:
			case TF_WEAPON_LUNCHBOX:
				return EWeaponType::PROJECTILE;
		}

		return EWeaponType::HITSCAN;
	}

	bool IsHitscan() { return GetWeaponType() == EWeaponType::HITSCAN; }

	bool CanPrimaryAttack()
	{
		CTFPlayer* owner = HandleAs<CTFPlayer*>(m_hOwnerEntity());
		if (!owner)
			return false;

		float curtime = TICKS_TO_TIME(owner->GetTickBase());
		return m_flNextPrimaryAttack() <= curtime && m_flNextAttack() <= curtime;
	}

	bool CanSecondaryAttack()
	{
		CTFPlayer* owner = HandleAs<CTFPlayer*>(m_hOwnerEntity());
		if (!owner)
			return false;

		float curtime = TICKS_TO_TIME(owner->GetTickBase());
		return m_flNextSecondaryAttack() && m_flNextAttack() <= curtime;
	}

	bool HasPrimaryAmmoForShot()
	{
		return m_iClip1() > 0 || m_iClip1() == -1;
	}

	bool IsMelee() { return GetSlot() == SLOT_MELEE; }

	bool CanAmbassadorHeadshot()
	{
		if (GetWeaponID() == TF_WEAPON_REVOLVER && AttributeHookValue(0, "set_weapon_mode", this, nullptr, true))
			return (interfaces::GlobalVars->curtime - m_flLastFireTime()) > 1.0f;
		return false;
	}

	bool IsSniperRifle()
	{
		switch (GetClassID())
		{
			case ETFClassID::CTFSniperRifleClassic:
			case ETFClassID::CTFSniperRifle:
			case ETFClassID::CTFSniperRifleDecap:
				return true;

			default:
				return false;
		}

		return false;
	}

	bool IsAmbassador()
	{
		return GetClassID() == ETFClassID::CTFRevolver
		&& (m_iItemDefinitionIndex() == Spy_m_FestiveAmbassador || m_iItemDefinitionIndex() == Spy_m_TheAmbassador);
	}
};

class CTFKnife : public CTFWeaponBase
{
public:
	NETVAR(m_bReadyToBackstab, "CTFKnife->m_bReadyToBackstab", bool);
	NETVAR(m_bKnifeExists, "CTFKnife->m_bKnifeExists", bool);
	NETVAR(m_flKnifeRegenerateDuration, "CTFKnife->m_flKnifeRegenerateDuration", float);
	NETVAR(m_flKnifeMeltTimestamp, "CTFKnife->m_flKnifeMeltTimestamp", float);
};

class CTFMinigun : public CTFWeaponBase
{
public:
	NETVAR(m_iWeaponState, "CTFMinigun->m_iWeaponState", int);
	NETVAR(m_bCritShot, "CTFMinigun->m_bCritShot", bool);
};

class CWeaponMedigun : public CTFWeaponBase
{
public:
	NETVAR(m_hHealingTarget, "CWeaponMedigun->m_hHealingTarget", EHANDLE);
	NETVAR(m_bHealing, "CWeaponMedigun->m_bHealing", bool);
	NETVAR(m_bAttacking, "CWeaponMedigun->m_bAttacking", bool);
	NETVAR(m_bChargeRelease, "CWeaponMedigun->m_bChargeRelease", bool);
	NETVAR(m_bHolstered, "CWeaponMedigun->m_bHolstered", bool);
	NETVAR(m_nChargeResistType, "CWeaponMedigun->m_nChargeResistType", int);
	NETVAR(m_hLastHealingTarget, "CWeaponMedigun->m_hLastHealingTarget", EHANDLE);
	NETVAR(m_flChargeLevel, "CWeaponMedigun->m_flChargeLevel", float);

	//int GetMedigunType();
	//MedigunChargeTypes GetChargeType();
	//medigun_resist_types_t GetResistType();
};

class CTFPipebombLauncher : public CTFWeaponBase
{
public:
	NETVAR(m_iPipebombCount, "CTFPipebombLauncher->m_iPipebombCount", int);
	NETVAR(m_flChargeBeginTime, "CTFPipebombLauncher->m_flChargeBeginTime", float);
	//int GetDetonateType();
};

class CTFSniperRifle : public CTFWeaponBase
{
public:
	NETVAR(m_flChargedDamage, "CTFSniperRifle->m_flChargedDamage", float);

	float GetChargedDamage()
	{
		bool isMachina = m_iItemDefinitionIndex() == Sniper_m_TheMachina;
		float mult = isMachina ? 1.15f : 1.0f;
		return m_flChargedDamage() * 3 * mult;
	}
};

class CTFGrenadeLauncher : public CTFWeaponBase
{
public:
	NETVAR(m_flDetonateTime, "CTFGrenadeLauncher->m_flDetonateTime", float);
	NETVAR(m_iCurrentTube, "CTFGrenadeLauncher->m_iCurrentTube", int);
	NETVAR(m_iGoalTube, "CTFGrenadeLauncher->m_iGoalTube", int);

	//int GetDetonateType();
};

class CTFSniperRifleClassic : public CTFSniperRifle
{
public:
	NETVAR(m_bCharging, "CTFSniperRifleClassic->m_bCharging", bool);
};

class CTFParticleCannon : public CTFWeaponBase
{
public:
	NETVAR(m_flChargeBeginTime, "CTFParticleCannon->m_flChargeBeginTime", float);
	NETVAR(m_iChargeEffect, "CTFParticleCannon->m_iChargeEffect", int);
};

class CTFFlareGun : public CTFWeaponBase
{
public:
	//int GetFlareGunType();
};

class CTFThrowable : public CTFWeaponBase
{
public:
	NETVAR(m_flChargeBeginTime, "CTFThrowable->m_flChargeBeginTime", float);
};

class CTFGrapplingHook : public CTFWeaponBase
{
public:
	NETVAR(m_hProjectile, "CTFGrapplingHook->m_hProjectile", EHANDLE);
};

class CTFSpellBook : public CTFThrowable
{
public:
	NETVAR(m_flTimeNextSpell, "CTFSpellBook->m_flTimeNextSpell", float);
	NETVAR(m_iSelectedSpellIndex, "CTFSpellBook->m_iSelectedSpellIndex", int);
	NETVAR(m_iSpellCharges, "CTFSpellBook->m_iSpellCharges", int);
	NETVAR(m_bFiredAttack, "CTFSpellBook->m_bFiredAttack", bool);
};