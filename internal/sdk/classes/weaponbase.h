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

struct ProjectileInfo_t
{
	float speed = 0;
	float gravity = 0;
	float primetime = 0;
	float damage_radius = 0;
	float lifetime = 60.0f;
	bool simple_trace = false;
	Vector offset{};
	Vector hull{6, 6, 6};
};

inline FileWeaponInfo_t* Rebuild_GetFileWeaponInfoFromHandle(void* handle)
{
	// GetFileWeaponInfoFromHandle 66 3B 3D D3 03 77 01 48 8D 05 72 FC 76 01 ? ? 48 8B 15 B1 03 77 01
	using GetFileWeaponInfoFromHandleFn = FileWeaponInfo_t*(*)(void*);
	static auto orig = (GetFileWeaponInfoFromHandleFn)sigscan_module("client.so", "66 3B 3D D3 03 77 01 48 8D 05 72 FC 76 01 ? ? 48 8B 15 B1 03 77 01");
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
		CTFPlayer* owner = HandleAs<CTFPlayer>(m_hOwnerEntity());
		if (!owner)
			return false;

		float curtime = TICKS_TO_TIME(owner->GetTickBase());
		return m_flNextPrimaryAttack() <= curtime && m_flNextAttack() <= curtime;
	}

	bool CanSecondaryAttack()
	{
		CTFPlayer* owner = HandleAs<CTFPlayer>(m_hOwnerEntity());
		if (!owner)
			return false;

		float curtime = TICKS_TO_TIME(owner->GetTickBase());
		return m_flNextSecondaryAttack() && m_flNextAttack() <= curtime;
	}

	bool HasPrimaryAmmoForShot()
	{
		return m_iClip1() > 0 || m_iClip1() == -1;
	}

	template<typename T>
	T As(T)
	{
		return static_cast<T>(this);
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

inline bool GetProjectileInfo(ProjectileInfo_t& info, CTFPlayer* owner, CTFWeaponBase* pWeapon)
{
	bool bDucking = owner->GetFlags() & FL_DUCKING;
	float gravity = interfaces::Cvar->FindVar("sv_gravity")->GetFloat()/800;

	int id = pWeapon->GetWeaponID();
	//interfaces::cvar->ConsolePrintf("Weapon ID: %d\n", id);

	int iTickBase = owner->GetTickBase();
	float flTickBase = TIME_TO_TICKS(iTickBase);

	switch(id)
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		{
			info.hull.Set();
			info.speed = owner->InCond(TF_COND_RUNE_PRECISION) ? 3000 : interfaces::attributeManager.AttributeHookValue(1100, "mult_projectile_speed", pWeapon);
			info.offset.x = 23.5f;
			info.offset.y = interfaces::attributeManager.AttributeHookValue(0, "centerfire_projectile", pWeapon) == 1 ? 0 : 12;
			info.offset.z = bDucking ? 8 : -3;
			info.damage_radius = id == TF_WEAPON_ROCKETLAUNCHER ? 146 : 44;
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_DRG_POMSON:
		{
			bool bIsCowMangler = id == TF_WEAPON_PARTICLE_CANNON;
			info.offset.Set(23.5, 8, bDucking ? 8 : -3);
			info.speed = bIsCowMangler ? 1100 : 1200;
			info.hull = bIsCowMangler ? Vector(0, 0, 0) : Vector(1, 1, 1);
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_CANNON:
		{
			bool bIsCannon = id == TF_WEAPON_CANNON;
			float mortar = bIsCannon ? interfaces::attributeManager.AttributeHookValue(0.f, "grenade_launcher_mortar_mode", pWeapon) : 0;
			info.speed = interfaces::attributeManager.AttributeHookValue(owner->InCond(TF_COND_RUNE_PRECISION) ? 3000 : interfaces::attributeManager.AttributeHookValue(1200, "mult_projectile_range", pWeapon), "mult_projectile_range", pWeapon);
			info.gravity = gravity;
			return true;
		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			info.offset.Set(16, 8, -6);
			info.gravity = gravity;
			
			float charge = 0.0f;
			float m_flChargeBeginTime = ((CTFPipebombLauncher*)pWeapon)->m_flChargeBeginTime();
			if (m_flChargeBeginTime > flTickBase)
				charge = 0.0f;
			else
				charge = flTickBase - m_flChargeBeginTime;

			info.speed = interfaces::attributeManager.AttributeHookValue(Math::RemapVal(0, 0, interfaces::attributeManager.AttributeHookValue(4.0f, "stickybomb_charge_rate", pWeapon), 900, 2400, true), "mult_projectile_range", pWeapon);
			return true;
		}

		case TF_WEAPON_FLAREGUN:
		{
			info.offset.Set(23.5, 12, bDucking ? 8 : -3);
			info.hull.Set(0, 0, 0);
			info.speed = interfaces::attributeManager.AttributeHookValue(2000, "mult_projectile_speed", pWeapon);
			info.gravity = 0;
			info.lifetime = 0.3 * gravity;
			return true;
		}

		case TF_WEAPON_FLAREGUN_REVENGE:
		{
			info.offset.Set(23.5, 12, bDucking ? 8 : -3);
			info.hull.Set(0, 0, 0);
			info.speed = 3000;
			return true;
		}

		case TF_WEAPON_COMPOUND_BOW:
		{
			info.offset.Set(23.5, 12, -3);
			info.hull.Set(1, 1, 1);

			float flchargebegintime = static_cast<CTFPipebombLauncher*>(pWeapon)->m_flChargeBeginTime();
			float charge = 0.0f;
			if (flchargebegintime > 0)
				charge = TICKS_TO_TIME( owner->GetTickBase()) - flchargebegintime;

			info.speed = Math::RemapVal(charge, 0, 1, 1800, 2600);
			info.gravity = Math::RemapVal(charge, 0, 1, 0.5, 0.1) * gravity;
			info.lifetime = 10;
			return true;
		}

		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			bool isCrossbow = id == TF_WEAPON_CROSSBOW;
			info.offset.Set(23.5, 12, -3);
			info.hull = isCrossbow ? Vector(3, 3, 3) : Vector(1, 1, 1);
			info.speed = 2400;
			info.gravity = gravity * 0.2;
			info.lifetime = 10;
			return true;
		}

		case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			info.offset.Set(16, 6, -8);
			info.hull.Set(1, 1, 1);
			info.speed = 1000;
			info.gravity = 0.3 * gravity;
			return true;
		}

		case TF_WEAPON_FLAMETHROWER:
		{
			static ConVar* tf_flamethrower_size = interfaces::Cvar->FindVar("tf_flamethrower_boxsize");
			if (!tf_flamethrower_size)
				return false;
			
			float flhull = tf_flamethrower_size->GetFloat();
			info.offset.Set(40, 5, 0);
			info.hull.Set(flhull, flhull, flhull);
			info.speed = 1000;
			info.lifetime = 0.285;
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_FLAME_BALL:
		{
			info.offset.Set(3, 7, -9);
			info.hull.Set(1, 1, 1);
			info.speed = 3000;
			info.lifetime = 0.18;
			info.gravity = 0;
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_CLEAVER:
		{
			info.offset.Set(16, 8, -6);
			info.hull.Set(1, 1, 10); // wtf is this 10?
			info.gravity = 1;
			info.lifetime = 2.2;
			return true;
		}

		case TF_WEAPON_BAT_WOOD:
		case TF_WEAPON_BAT_GIFTWRAP:
		{
			ConVar* tf_scout_stunball_base_speed = interfaces::Cvar->FindVar("tf_scout_stunball_base_speed");
			if (!tf_scout_stunball_base_speed)
				return false;

			info.speed = tf_scout_stunball_base_speed->GetFloat();
			info.gravity = 1;
			info.lifetime = gravity;
			return true;
		}

		case TF_WEAPON_JAR:
		case TF_WEAPON_JAR_MILK:
		{
			info.offset.Set(16, 8, -6);
			info.speed = 1000;
			info.gravity = 1;
			info.lifetime = 2.2;
			info.hull.Set(3, 3, 3);
			return true;
		}

		case TF_WEAPON_JAR_GAS:
		{
			info.offset.Set(16, 8, -6);
			info.speed = 2000;
			info.gravity = 1;
			info.lifetime = 2.2;
			info.hull.Set(3, 3, 3);
			return true;
		}

		case TF_WEAPON_LUNCHBOX:
		{
			info.offset.z = -8;
			info.hull.Set(17, 17, 7);
			info.speed = 500;
			info.gravity = 1 * gravity;
			return true;
		}
		default: return false;
	}

	return false;
}