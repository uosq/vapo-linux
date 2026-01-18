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
		uintptr_t handleAddress = (uintptr_t)this + 0xf12;
		void* handleValue = *(void**)handleAddress;
		return Rebuild_GetFileWeaponInfoFromHandle(handleValue);
	}

	//int GetSlot() { return GetWeaponInfo()->iSlot; }
	int GetWeaponID() {
		// this is stupid
		using Fn = int(*)(void*);
		auto vtable = *reinterpret_cast<void***>(this);
		auto fn = reinterpret_cast<Fn>(vtable[0xE18 / sizeof(void*)]);
		return fn(this);
	}

	//bool IsEnergyWeapon() { return vtable::call<436, bool>(this); }
	//bool CalcIsAttackCriticalHelper() { return vtable::call<400, bool>(this); }
	//bool AreRandomCritsEnabled() { return vtable::call<406, bool>(this); }
	//float GetWeaponSpread() { return vtable::call<471, float>(this); }
	//int GetSwingRange() { return vtable::call<459, int>(this); }

	bool IsInReload()
	{
		return m_bInReload() || m_iReloadMode() != 0;
	}

	int GetSlot()
	{
		return GetWeaponInfo()->iSlot;
	}

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

	bool IsHitscan()
	{
		return GetWeaponType() == EWeaponType::HITSCAN;
	}

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

	bool IsMelee()
	{
		return GetSlot() == SLOT_MELEE;
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

	//int GetRifleType();
	//float GetHeadshotMult(CTFPlayer* pTarget = nullptr);
	//float GetBodyshotMult(CTFPlayer* pTarget = nullptr);
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